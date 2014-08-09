#include "Platform.hpp"
#include "Settings.hpp"
#include "GtkAssociationDialog.hpp"
#include "GtkMainWindow.hpp"


/**
* Sets up the main window.
*/
GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	//TODO:This needs to be refactored
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(800, 500);
	Gtk::Paned *panel = Gtk::manage(new Gtk::Paned(Gtk::ORIENTATION_VERTICAL));

	m_infobar =  Gtk::manage(new GtkTorrentInfoBar());
	m_treeview = Gtk::manage(new GtkTorrentTreeView(m_infobar));

	panel->pack1(*m_treeview);
	panel->pack2(*m_infobar);

	Glib::     signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1000);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);
	header->set_title("gTorrent");

	Gtk::Button *btn_add_link    = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_add_torrent = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_pause       = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_remove      = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_resume      = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_settings    = Gtk::manage(new Gtk::Button());
	Gtk::VSeparator *separator  = Gtk::manage(new Gtk::VSeparator());

	btn_add_link   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	btn_add_torrent->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	btn_pause      ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	btn_resume     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));
	btn_remove     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onRemoveBtnClicked));
	btn_settings   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onSettingsBtnClicked));

	btn_add_link   ->set_image_from_icon_name("edit-paste");
	btn_add_torrent->set_image_from_icon_name("gtk-add");
	btn_pause      ->set_image_from_icon_name("media-playback-pause");
	btn_remove     ->set_image_from_icon_name("gtk-cancel");
	btn_resume     ->set_image_from_icon_name("media-playback-start");
	btn_settings   ->set_image_from_icon_name("emblem-system-symbolic");

	header->add(*btn_add_torrent);
	header->add(*btn_add_link);
	header->add(*separator);
	header->add(*btn_pause);
	header->add(*btn_resume);
	header->add(*btn_remove);
	header->pack_end(*btn_settings);

	// Let's add some DnD goodness
	vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));
	listTargets.push_back(Gtk::TargetEntry("text/uri-list"));
	listTargets.push_back(Gtk::TargetEntry("application/x-bittorrent"));

	m_treeview->drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE);
	m_treeview->signal_drag_data_received().connect(sigc::mem_fun(*this, &GtkMainWindow::onFileDropped));

	set_titlebar(*header);
	add(*panel);
	show_all();
	m_infobar->set_visible(false);

	if (gt::Settings::getOptionAsString("FileAssociation") == "" ||
		gt::Settings::getOptionAsInt("FileAssociation") == -1)
	{
		GtkAssociationDialog *dialog = new GtkAssociationDialog(*this);
		int code = dialog->run();// code = -1 (Remind me later), 0(Do not associate), 1(Associate with torrents), 2(Associate with magnets), 3(Assiciate with both)
		if(code != -1)
			gt::Platform::associate(code & 2, code & 1);
		gt::Settings::setOption("FileAssociation", code);
		delete dialog;
	}

	d = new GtkSettingsDialog(this);
}

/**
* Does something when a file is dropped onto the window.
*/
void GtkMainWindow::onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	string sel_data = selection_data.get_data_as_string();
	if(m_core->isMagnetLink(sel_data))
	{
		shared_ptr<gt::Torrent> t = m_core->addTorrent(sel_data);
		if (t)//Checks if t is not null
			m_treeview->addCell(t);
	}
	else
	{
		string fn = Glib::filename_from_uri(sel_data);
		boost::algorithm::trim(fn); //d-don't worry guys! w-we only need boo-boost for libtorrent! th-that's all!
		bool want_uncertain = true;
		string content_type = Gio::content_type_guess(fn, sel_data, want_uncertain);
		if(content_type == "application/x-bittorrent" || content_type == ".torrent")
		{
			shared_ptr<gt::Torrent> t = m_core->addTorrent(fn);
			if (t)//Checks if t is not null
				m_treeview->addCell(t);
			//TODO Add error dialogue if torrent add is unsuccessful
		}
	}
}

/**
* Does something each second.
*/
bool GtkMainWindow::onSecTick()
{
	m_treeview->updateCells();
	m_infobar->updateInfo(m_treeview->getFirstSelected());
	shared_ptr<gt::Torrent> t = m_core->update();
	if (t)
		m_treeview->addCell(t);

	return true;
}

/**
* Does something when the add button is clicked.
*/
void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse for torrent file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_default_size(256, 256);
	fc.set_select_multiple();
	fc.set_transient_for(*this);
	fc.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	fc.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_t = Gtk::FileFilter::create();
	filter_t->set_name("Torrent Files");
	filter_t->add_mime_type("application/x-bittorrent");
	fc.add_filter(filter_t);

	int result = fc.run();

	switch (result)
	{
	case Gtk::RESPONSE_OK:
		for (auto & f : fc.get_filenames())
		{
			shared_ptr<gt::Torrent> t = m_core->addTorrent(f);
			if (t)//Checks if t is not null
				m_treeview->addCell(t);
			//TODO Add error dialogue if torrent add is unsuccessful
		}
		break;
	}
}

/**
* Does something when the add magnet button is clicked.
*/
void GtkMainWindow::onAddMagnetBtnClicked()
{
	GtkAddMagnetLinkWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r)
	{
	case Gtk::RESPONSE_OK:
		shared_ptr<gt::Torrent> t = m_core->addTorrent(d.getMagnetURL());
		if (t)
			m_treeview->addCell(t);
		//TODO Add error dialogue if torrent add is unsuccessful
		break;
	}
}

/**
* Does something when the pause button is clicked.
*/
void GtkMainWindow::onPauseBtnClicked()
{
	m_treeview->setSelectedPaused(true);
}

/**
* Does something when the resume button is clicked.
*/
void GtkMainWindow::onResumeBtnClicked()
{
	m_treeview->setSelectedPaused(false);
}

/**
* Does something when the remove button is clicked.
*/
void GtkMainWindow::onRemoveBtnClicked()
{
	m_treeview->removeSelected();
}

/**
* Does something when the properties button is clicked.
*/
void GtkMainWindow::onPropertiesBtnClicked()
{
}

/**
* Does something when the window is destroyed. // That's some 10/10 doc right there.
*/
bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}

void GtkMainWindow::onSettingsBtnClicked()
{
	d->run();
}
