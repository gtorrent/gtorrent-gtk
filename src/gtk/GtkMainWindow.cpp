#include "GtkMainWindow.hpp"

#include <gtorrent/Platform.hpp>
#include <gtorrent/Settings.hpp>

#include <boost/algorithm/string.hpp>
#include <giomm.h>
#include <glibmm.h>
#include <libnotify/notify.h>

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/main.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/stock.h>

#include "../Application.hpp"
#include "GtkAssociationDialog.hpp"
#include "GtkTorrentTreeView.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkSettingsDialog.hpp"

/**
* Sets up the main window.
*/
GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	//TODO:This needs to be refactored
	notify_init ("Hello world!");
	set_position(Gtk::WIN_POS_CENTER);
	set_default_size(800, 500);
	magtxt->set_visible();
	magtxt->set_width_chars(75);
	magPop->add(*magtxt);
	btn_add_link->set_popover(*magPop);

	Gtk::Paned *panel = Gtk::manage(new Gtk::Paned(Gtk::ORIENTATION_VERTICAL));
	m_swin = Gtk::manage(new Gtk::ScrolledWindow());

	m_infobar =  Gtk::manage(new GtkTorrentInfoBar());
	m_treeview = Gtk::manage(new GtkTorrentTreeView(this, m_infobar));
	m_infobar->set_margin_left(5);
	m_infobar->set_margin_right(5);

	m_swin->add(*m_treeview);
	panel->pack1(*m_swin);
	panel->pack2(*m_infobar);

	Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1);
	signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));
	signal_key_press_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onKeyPress), false);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);
	header->set_title("gTorrent");

	Gtk::VSeparator *separator0  = Gtk::manage(new Gtk::VSeparator());
	Gtk::VSeparator *separator2  = Gtk::manage(new Gtk::VSeparator());

	btn_add_link   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	btn_add_torrent->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	btn_pause      ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	btn_resume     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));
	btn_remove     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onRemoveBtnClicked));
	btn_settings   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onSettingsBtnClicked));
	btn_properties ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPropertiesBtnClicked));

	btn_add_link   ->set_image_from_icon_name("insert-link-symbolic");
	btn_add_torrent->set_image_from_icon_name("list-add-symbolic");
	btn_pause      ->set_image_from_icon_name("media-playback-pause-symbolic");
	btn_properties ->set_image_from_icon_name("preferences-system-symbolic");
	btn_remove     ->set_image_from_icon_name("edit-delete-symbolic");
	btn_resume     ->set_image_from_icon_name("media-playback-start-symbolic");
	btn_settings   ->set_image_from_icon_name("emblem-system-symbolic");

	//TODO:align properties button to right of top bar
	//btn_properties->set_alignment(1.0f,0.0f);
	header->add(*btn_add_torrent);
	header->add(*btn_add_link);
	header->add(*separator0);
	header->add(*btn_resume);
	header->add(*btn_pause);
	header->add(*btn_remove);
	header->add(*separator1);
	header->add(*btn_properties);
	header->add(*separator2);
	header->pack_end(*btn_settings);
	// Let's add some DnD goodness
	std::vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));
	listTargets.push_back(Gtk::TargetEntry("text/uri-list"));
	listTargets.push_back(Gtk::TargetEntry("application/x-bittorrent"));

	m_treeview->drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE);
	m_treeview->signal_drag_data_received().connect(sigc::mem_fun(*this, &GtkMainWindow::onFileDropped));

	set_titlebar(*header);
	add(*panel);
	show_all();
	btn_pause->hide();
	m_infobar->set_visible(false);

	// for some reason, the treeview start with its first element selected
	m_treeview->get_selection()->unselect_all();

	for(auto tor : Application::getSingleton()->getCore()->getTorrents())
	{
		tor->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
		m_treeview->addCell(tor);
	}
	gt::Log::Debug(gt::Settings::settings["FileAssociation"].c_str());

	if (gt::Settings::settings["FileAssociation"] == "" ||
	        gt::Settings::settings["FileAssociation"] == "-1")
	{
		GtkAssociationDialog *dialog = new GtkAssociationDialog(*this);
		int code = dialog->run();// code = -1 (Remind me later), 0(Do not associate), 1(Associate with torrents), 2(Associate with magnets), 3(Assiciate with both)
		if(code != -1)
			gt::Platform::associate(code & 2, code & 1);
		gt::Settings::settings["FileAssociation"] = std::to_string(code);
		delete dialog;
	}

	d = new GtkSettingsDialog(this);
}

/**
* Does something when a file is dropped onto the window.
*/
void GtkMainWindow::onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	std::string sel_data = selection_data.get_data_as_string();
	if(m_core->isLink(sel_data))
	{
		std::shared_ptr<gt::Torrent> t = m_core->addTorrent(sel_data);
		if (t)//Checks if t is not null
			m_treeview->addCell(t);
	}
	else
	{
		std::string fn = Glib::filename_from_uri(sel_data);
		boost::algorithm::trim(fn); //d-don't worry guys! w-we only need boo-boost for libtorrent! th-that's all!
		bool want_uncertain = true;
		std::string content_type = Gio::content_type_guess(fn, sel_data, want_uncertain);
		if(content_type == "application/x-bittorrent" || content_type == ".torrent")
		{
			std::shared_ptr<gt::Torrent> t = m_core->addTorrent(fn);
			if (t)//Checks if t is not null
			{
				t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
				m_treeview->addCell(t);
			}
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
	m_infobar->updateState(m_treeview->getFirstSelected());
	std::shared_ptr<gt::Torrent> t = m_core->update();
	if (t)
	{
		t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
		m_treeview->addCell(t);
	}
	m_swin->get_vscrollbar()->set_child_visible(false);
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
			std::shared_ptr<gt::Torrent> t = m_core->addTorrent(f);
			if (t)//Checks if t is not null
			{
				t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
				m_treeview->addCell(t);
			}
			//TODO Add error dialogue if torrent add is unsuccessful
		}
		break;
	}
}

void GtkMainWindow::torrentStateChangedCallback(int oldstate, std::shared_ptr<gt::Torrent> t)
{
	NotifyNotification *Hello = nullptr;

	int newstate = t->getState();
	if(newstate == libtorrent::torrent_status::seeding && oldstate == libtorrent::torrent_status::downloading)
		Hello = notify_notification_new (t->getName().c_str(), std::string(t->getName() + " has finished downloading.").c_str(), "dialog-information");
	else if(newstate == libtorrent::torrent_status::downloading  && 
			oldstate == libtorrent::torrent_status::downloading_metadata)
		Hello = notify_notification_new (t->getName().c_str(), std::string(t->getName() + " has started downloading.").c_str(), "dialog-information");
	else 
		return; //:^)

	notify_notification_show (Hello, NULL);
	g_object_unref(G_OBJECT(Hello));
}

/**
* Does something when the add magnet button is clicked.
*/
void GtkMainWindow::onAddMagnetBtnClicked()
{
	if(magPop->get_visible())
	{
		Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
		std::string link = clip->wait_for_text();
		if(gt::Core::isLink(link))
			magtxt->set_text(link);
	}
	else
	{
		std::shared_ptr<gt::Torrent> t = m_core->addTorrent(magtxt->get_text());
		if (t)
		{
			t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
			m_treeview->addCell(t);
		}
		magtxt->set_text("");
	}
}

/**
* Does something when the pause button is clicked.
*/
void GtkMainWindow::onPauseBtnClicked()
{
	m_treeview->setSelectedPaused(true);
	m_treeview->onSelectionChanged();
}

/**
* Does something when the resume button is clicked.
*/
void GtkMainWindow::onResumeBtnClicked()
{
	m_treeview->setSelectedPaused(false);
	m_treeview->onSelectionChanged();
}

/**
* Does something when the remove button is clicked.
*/
void GtkMainWindow::onRemoveBtnClicked()
{
	for(auto t : m_treeview->selectedTorrents())
		m_infobar->removeInfo(t);
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
	m_treeview->saveColumns();
	notify_uninit();
	m_core->shutdown();
	return false;
}

void GtkMainWindow::onSettingsBtnClicked()
{
	d->run();
}

bool GtkMainWindow::onKeyPress(GdkEventKey *event)
{
	short arrowkeys[] = { 80, 88, 83, 85, 111, 114, 113, 116 };
	short enter[] = { 36, 104 };
	if(std::find(arrowkeys, arrowkeys + 8, event->hardware_keycode) == arrowkeys + 8) 
	{
		m_treeview->m_searchPopover->set_visible();
		if(std::find(enter, enter + 2, event->hardware_keycode) != enter + 2)
			m_treeview->m_searchPopover->set_visible(false);
		m_treeview->m_filter->refilter();
	}
	return false;
}
