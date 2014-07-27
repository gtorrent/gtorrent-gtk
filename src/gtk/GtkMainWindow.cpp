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

	this->set_icon_from_file("gtorrent.png");

	m_infobar =  Gtk::manage(new GtkTorrentInfoBar());
	m_treeview = Gtk::manage(new GtkTorrentTreeView(m_infobar));

	panel->pack1(*m_treeview);
	panel->pack2(*m_infobar);

	Glib::     signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1000);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);
	header->set_title("gTorrent");

	Glib::RefPtr<Gtk::ActionGroup> action_group = Gtk::ActionGroup::create();

	action_group->add(Gtk::Action::create( "Properties", Gtk::Stock::PROPERTIES));
	action_group->add(Gtk::Action::create(   "Add Link", Gtk::Stock::PASTE),       sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	action_group->add(Gtk::Action::create("Add Torrent", Gtk::Stock::ADD),         sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	action_group->add(Gtk::Action::create(     "Remove", Gtk::Stock::CANCEL),      sigc::mem_fun(*this, &GtkMainWindow::onRemoveBtnClicked));
	action_group->add(Gtk::Action::create(     "Resume", Gtk::Stock::MEDIA_PLAY),  sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));
	action_group->add(Gtk::Action::create(      "Pause", Gtk::Stock::MEDIA_PAUSE), sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	action_group->add(Gtk::Action::create(         "Up", Gtk::Stock::GO_UP));
	action_group->add(Gtk::Action::create(       "Down", Gtk::Stock::GO_DOWN));

	Glib::RefPtr<Gtk::UIManager> ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(action_group);
	add_accel_group(ui_manager->get_accel_group());

	Glib::ustring ui_info =
	    "<ui>"
	    "	<toolbar  name='ToolBar'>"
	    "		<toolitem action='Properties' />"
	    "		<separator />"
	    "		<toolitem action='Add Link' />"
	    "		<toolitem action='Add Torrent' />"
	    "		<separator />"
	    "		<toolitem action='Remove' />"
	    "		<toolitem action='Pause' />"
	    "		<toolitem action='Resume' />"
	    "		<separator />"
	    "		<toolitem action='Down' />"
	    "		<toolitem action='Up' />"
	    "		<separator />"
	    "	</toolbar>"
	    "</ui>";

	ui_manager->add_ui_from_string(ui_info);
	this->add(*panel);
	Gtk::Widget* pToolBar = ui_manager->get_widget("/ToolBar");
	pToolBar->override_background_color(Gdk::RGBA("0, 0, 0, 0"));
	header->add(*pToolBar);

	// Let's add some DnD goodness
	vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));
	listTargets.push_back(Gtk::TargetEntry("text/uri-list"));
	listTargets.push_back(Gtk::TargetEntry("application/x-bittorrent"));

	m_treeview->drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE);
	m_treeview->signal_drag_data_received().connect(sigc::mem_fun(*this, &GtkMainWindow::onFileDropped));

	this->set_titlebar(*header);
	this->show_all();

	panel->set_position(this->get_height() * 0.5);
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
		boost::algorithm::trim(fn);
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
		if (t)//Checks if t is not null
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
* Does something when the window is destroyed.
*/
bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
