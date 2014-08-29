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
GtkMainWindow::GtkMainWindow(GtkWindow *win, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Window(win), builder(rbuilder), m_core(Application::getSingleton()->getCore())
{
	//TODO:This needs to be refactored
	notify_init ("Hello world!");
	set_position(Gtk::WIN_POS_CENTER);
	set_default_size(800, 500);
	magtxt->set_visible();
	magtxt->set_width_chars(75);
	magPop->add(*magtxt);
	btn_add_link->set_popover(*magPop);

	Gtk::Box *vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
	add(*vbox);
	Gtk::Paned *panel = Gtk::manage(new Gtk::Paned(Gtk::ORIENTATION_VERTICAL));

	Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar());
	vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

	Gtk::MenuItem *menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
	menubar->append(*menuitem_file);
	Gtk::MenuItem *menuitem_options = Gtk::manage(new Gtk::MenuItem("_Options", true));
	menubar->append(*menuitem_options);
	Gtk::MenuItem *menuitem_help = Gtk::manage(new Gtk::MenuItem("_Help", true));
	menubar->append(*menuitem_help);

	Gtk::Menu *filemenu = Gtk::manage(new Gtk::Menu());
	menuitem_file->set_submenu(*filemenu);
	Gtk::MenuItem *menuitem_add_torrent = Gtk::manage(new Gtk::MenuItem("_Add Torrent...", true));
	filemenu->append(*menuitem_add_torrent);
	Gtk::MenuItem *menuitem_add_torrent_nds = Gtk::manage(new Gtk::MenuItem("_Add Torrent (no default save)...", true));
	filemenu->append(*menuitem_add_torrent_nds);
	Gtk::MenuItem *menuitem_add_torrent_from_url = Gtk::manage(new Gtk::MenuItem("_Add Torrent from URL...", true));
	filemenu->append(*menuitem_add_torrent_from_url);
	Gtk::MenuItem *menuitem_add_rss_feed = Gtk::manage(new Gtk::MenuItem("_Add RSS Feed...", true));
	filemenu->append(*menuitem_add_rss_feed);
	Gtk::MenuItem *menuitem_create_new_torrent = Gtk::manage(new Gtk::MenuItem("_Create New Torrent...", true));
	filemenu->append(*menuitem_create_new_torrent);
	Gtk::MenuItem *menuitem_quit = Gtk::manage(new Gtk::MenuItem("_Exit", true));
	filemenu->append(*menuitem_quit);

	Gtk::Menu *optionsmenu = Gtk::manage(new Gtk::Menu());
	menuitem_options->set_submenu(*optionsmenu);
	Gtk::MenuItem *menuitem_preferences = Gtk::manage(new Gtk::MenuItem("_Preferences", true));
	optionsmenu->append(*menuitem_preferences);
	Gtk::MenuItem *menuitem_rss_downloader = Gtk::manage(new Gtk::MenuItem("_RSS Downloader", true));
	optionsmenu->append(*menuitem_rss_downloader);
	Gtk::MenuItem *menuitem_setup_guide = Gtk::manage(new Gtk::MenuItem("_Setup Guide", true));
	optionsmenu->append(*menuitem_setup_guide);
	Gtk::MenuItem *menuitem_show_toolbar = Gtk::manage(new Gtk::MenuItem("_Show Toolbar", true));
	optionsmenu->append(*menuitem_show_toolbar);
	Gtk::MenuItem *menuitem_show_detailed_info = Gtk::manage(new Gtk::MenuItem("_Show Detailed Info", true));
	optionsmenu->append(*menuitem_show_detailed_info);
	Gtk::MenuItem *menuitem_show_status_bar = Gtk::manage(new Gtk::MenuItem("_Show Status Bar", true));
	optionsmenu->append(*menuitem_show_status_bar);
	Gtk::MenuItem *menuitem_show_category_list = Gtk::manage(new Gtk::MenuItem("_Show Category List", true));
	optionsmenu->append(*menuitem_show_category_list);
	Gtk::MenuItem *menuitem_icons_on_tabs = Gtk::manage(new Gtk::MenuItem("_Icons on Tabs", true));
	optionsmenu->append(*menuitem_icons_on_tabs);
	Gtk::MenuItem *menuitem_auto_shutdown = Gtk::manage(new Gtk::MenuItem("_Auto Shutdown", true));
	optionsmenu->append(*menuitem_auto_shutdown);

	Gtk::Menu *helpmenu = Gtk::manage(new Gtk::Menu());
	menuitem_help->set_submenu(*helpmenu);
	Gtk::MenuItem *menuitem_gtorrent_help = Gtk::manage(new Gtk::MenuItem("_gTorrent Help", true));
	helpmenu->append(*menuitem_gtorrent_help);
	Gtk::MenuItem *menuitem_gtorrent_faq = Gtk::manage(new Gtk::MenuItem("_gTorrent FAQ", true));
	helpmenu->append(*menuitem_gtorrent_faq);
	Gtk::MenuItem *menuitem_gtorrent_webpage = Gtk::manage(new Gtk::MenuItem("_gTorrent Webpage", true));
	helpmenu->append(*menuitem_gtorrent_webpage);
	Gtk::MenuItem *menuitem_gtorrent_forums = Gtk::manage(new Gtk::MenuItem("_gTorrent Forums", true));
	helpmenu->append(*menuitem_gtorrent_forums);
	Gtk::MenuItem *menuitem_check_for_updates = Gtk::manage(new Gtk::MenuItem("_Check for Updates", true));
	helpmenu->append(*menuitem_check_for_updates);
	Gtk::MenuItem *menuitem_download_translation = Gtk::manage(new Gtk::MenuItem("_Download Translation", true));
	helpmenu->append(*menuitem_download_translation);
	Gtk::MenuItem *menuitem_show_statistics = Gtk::manage(new Gtk::MenuItem("_Show Statistics", true));
	helpmenu->append(*menuitem_show_statistics);
	Gtk::MenuItem *menuitem_about_gtorrent = Gtk::manage(new Gtk::MenuItem("_About gTorrent", true));
	helpmenu->append(*menuitem_about_gtorrent);

	m_status_label = Gtk::manage(new Gtk::Label());
	m_status_label->set_markup("<b>Status Bar</b>");
	m_status_bar->add(*m_status_label);
	vbox->pack_end(*m_status_bar, Gtk::PACK_SHRINK, 0);

	magtxt->set_visible();
	magtxt->set_width_chars(75);
	magPop->add(*magtxt);
	btn_add_link->set_popover(*magPop);

	m_swin = Gtk::manage(new Gtk::ScrolledWindow());

	notify_init ("gTorrent");

	builder->get_widget( "addTorrentButton", addTorrentButton);
	builder->get_widget(  "addMagnetButton",  addMagnetButton);
	builder->get_widget(     "resumeButton",     resumeButton);
	builder->get_widget(      "pauseButton",      pauseButton);
	builder->get_widget(     "deleteButton",     removeButton);
	builder->get_widget("preferencesButton", propertiesButton);
	builder->get_widget(   "settingsButton",   settingsButton);
	builder->get_widget(            "panel",            panel);
	builder->get_widget(   "scrolledWindow",   scrolledWindow);
	builder->get_widget(          "vSepOne",    vSeparatorOne);
	builder->get_widget(          "vSepTwo",    vSeparatorTwo);
	builder->get_widget_derived(  "infobar",        m_infobar);

	magEntry   = Gtk::manage(new Gtk::Entry());
	magEntry->set_visible();
	magEntry->set_width_chars(75);
	magPopover = Gtk::manage(new Gtk::Popover());

	magPopover->add(*magEntry);
	magPopover->set_relative_to(*addMagnetButton);
	addMagnetButton->set_popover(*magPopover);

	m_treeview = Gtk::manage(new GtkTorrentTreeView(this, m_infobar));

	m_infobar->set_margin_left(5);
	m_infobar->set_margin_right(5);
	m_infobar->set_visible();

	m_treeview->set_visible();
	scrolledWindow->add(*m_treeview);
	panel->pack1(*scrolledWindow);

	Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1);
	signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));
	signal_key_press_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onKeyPress), false);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);
	header->set_title("gTorrent");

	Gtk::VSeparator *separator0  = Gtk::manage(new Gtk::VSeparator());
	Gtk::VSeparator *separator2  = Gtk::manage(new Gtk::VSeparator());
	Gtk::VSeparator *separator3  = Gtk::manage(new Gtk::VSeparator());
	Gtk::VSeparator *separator4  = Gtk::manage(new Gtk::VSeparator());

	btn_add_link   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	btn_add_torrent->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));

	btn_pause      ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	btn_resume     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));
	btn_remove     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onRemoveBtnClicked));

	btn_settings   ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onSettingsBtnClicked));

	btn_add_link   ->set_image_from_icon_name("insert-link-symbolic");
	btn_add_torrent->set_image_from_icon_name("list-add-symbolic");
	btn_add_rss->set_image_from_icon_name("folder-remote-symbolic");

	btn_create_torrent->set_image_from_icon_name("text-x-generic");

	btn_remove     ->set_image_from_icon_name("edit-delete-symbolic");

	btn_resume     ->set_image_from_icon_name("media-playback-start-symbolic");
	btn_pause      ->set_image_from_icon_name("media-playback-pause-symbolic");
	btn_stop      	->set_image_from_icon_name("media-playback-stop-symbolic");

	btn_up      	->set_image_from_icon_name("go-up");
	btn_down      	->set_image_from_icon_name("go-down");

	btn_settings   ->set_image_from_icon_name("emblem-system-symbolic");

	//TODO:align properties button to right of top bar
	//btn_properties->set_alignment(1.0f,0.0f);
	header->add(*btn_add_torrent);
	header->add(*btn_add_link);
	header->add(*btn_add_rss);

	header->add(*separator0);

	header->add(*btn_create_torrent);

	header->add(*separator1);

	header->add(*btn_remove);

	header->add(*separator2);

	header->add(*btn_resume);
	header->add(*btn_pause);
	header->add(*btn_stop);

	header->add(*separator3);

	header->add(*btn_up);
	header->add(*btn_down);

	header->add(*separator4);

	header->add(*btn_settings);
	//header->pack_end(*btn_settings);
	addTorrentButton->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked       ));
	pauseButton     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked     ));
	resumeButton    ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked    ));
	removeButton    ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onRemoveBtnClicked    ));
	settingsButton  ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onSettingsBtnClicked  ));
	addMagnetButton ->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked ));
	propertiesButton->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPropertiesBtnClicked));

	// Let's add some DnD goodness
	std::vector<Gtk::TargetEntry> listTargets;
	listTargets.push_back(Gtk::TargetEntry("STRING"));
	listTargets.push_back(Gtk::TargetEntry("text/plain"));
	listTargets.push_back(Gtk::TargetEntry("text/uri-list"));
	listTargets.push_back(Gtk::TargetEntry("application/x-bittorrent"));

	m_treeview->drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE);
	m_treeview->signal_drag_data_received().connect(sigc::mem_fun(*this, &GtkMainWindow::onFileDropped));

	vbox->add(*header);

	//set_titlebar(*header);

	vbox->add(*panel);

	//add(*panel);
	show_all();
	//btn_pause->hide();
	//m_infobar->set_visible(false);

	// for some reason, the treeview start with its first element selected

	m_treeview->get_selection()->unselect_all();

	for(auto tor : Application::getSingleton()->getCore()->getTorrents())
	{
		tor->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
		m_treeview->addCell(tor);
	}

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
	scrolledWindow->get_vscrollbar()->set_child_visible(false);
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
	if(magPopover->get_visible())
	{
		Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
		std::string link = clip->wait_for_text();
		if(gt::Core::isLink(link))
			magEntry->set_text(link);
	}
	else
	{
		std::shared_ptr<gt::Torrent> t = m_core->addTorrent(magEntry->get_text());
		if (t)
		{
			t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, this, std::placeholders::_1, std::placeholders::_2);
			m_treeview->addCell(t);
		}
		magEntry->set_text("");
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
	hide();
//	m_treeview->saveColumns();
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
