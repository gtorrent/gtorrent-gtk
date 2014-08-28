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
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

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
