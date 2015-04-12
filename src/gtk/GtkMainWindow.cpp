#include <future>
#include <boost/algorithm/string.hpp>

#include <libnotify/notify.h>

#include <giomm.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/main.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrollbar.h>

#include <gtorrent/Platform.hpp>
#include <gtorrent/Settings.hpp>
#include <gtorrent/Core.hpp>

#include "../Application.hpp"
#include "GtkAssociationDialog.hpp"
#include "GtkTorrentTreeView.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkSettingsDialog.hpp"
#include "GtkTorrentSideBar.hpp"
#include "GtkMainWindow.hpp"

// TODO: GtkRevealer should contain a listbox, list view of even a tree view like nautilus's
//       so remove the header, cutomize the cells, and enable the revealer with the property button
//       Also find a way to darken the treeview when the sidebar is shown, the same way the window is
//       darkened when a modal dialog is opened.

/**
* Sets up the main window.
*/
GtkMainWindow::GtkMainWindow(GtkWindow *win, const Glib::RefPtr<Gtk::Builder> rbuilder)
: Gtk::Window(win),
  builder(rbuilder),
  m_core(Application::getSingleton()->getCore())
{
	notify_init ("gTorrent");

	// Show all children at start so that widgets that need to hide can do so.
	show_all_children();

	Gtk::Revealer       *revealer;
	builder->get_widget("torrentbox", m_torrentbox);
	builder->get_widget("searchbar", m_searchbar);
	builder->get_widget("addTorrentButton", addTorrentButton);
	builder->get_widget("searchButton", m_searchButton);
	builder->get_widget("addMagnetButton", addMagnetButton);
	builder->get_widget("resumeButton", resumeButton);
	builder->get_widget("pauseButton", pauseButton);
	builder->get_widget("deleteButton", removeButton);
	builder->get_widget("preferencesButton", propertiesButton);
	builder->get_widget("buttonRss", buttonRss);
	builder->get_widget("settingsButton", settingsButton);
	builder->get_widget("panel", panel);
	builder->get_widget("scrolledWindow", scrolledWindow);
	builder->get_widget("vSepOne", vSeparatorOne);
	builder->get_widget("vSepTwo", vSeparatorTwo);
	builder->get_widget("sidebarRev", revealer);
	builder->get_widget("sidebarscroll", sidebar_scrolledwindow);
	builder->get_widget("main_stack", main_stack);
	builder->get_widget_derived("infobar", m_infobar);
	builder->get_widget_derived("treeview_torrent", m_treeview_torrent);
	builder->get_widget_derived("treeview_rss", m_treeview_rss);
	builder->get_widget_derived("sidebar", m_sidebar);
	builder->get_widget_derived("rssDialog", m_rss2);

	panel->pack2(*m_infobar);

	// Apparently can't use lambdas on these two unless doing something awful
	Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1);
	signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	addTorrentButton->signal_clicked().connect([this](){        onAddBtnClicked();});
	pauseButton     ->signal_clicked().connect([this](){      onPauseBtnClicked();});
	resumeButton    ->signal_clicked().connect([this](){     onResumeBtnClicked();});
	removeButton    ->signal_clicked().connect([this](){     onRemoveBtnClicked();});
	settingsButton  ->signal_clicked().connect([this](){   onSettingsBtnClicked();});
	addMagnetButton ->signal_clicked().connect([this](){  onAddMagnetBtnClicked();});
	m_searchButton  ->signal_clicked().connect([this](){m_searchbar->set_search_mode(!m_searchbar->get_search_mode());});
	propertiesButton->signal_clicked().connect([revealer](){ revealer->set_reveal_child(!revealer->get_reveal_child());});
//	buttonRss->signal_clicked().connect([this](){m_rss2->run("gTorrent RSS");});
	buttonRss->signal_clicked().connect([this](){main_stack->set_visible_child(*m_treeview_rss);});

	magPopover = Gtk::manage(new Gtk::Popover());
	magPopover->set_relative_to(*addMagnetButton);
	magPopover->set_position(Gtk::POS_LEFT);
	magEntry = Gtk::manage(new Gtk::Entry());
	magEntry->set_visible();
	magEntry->set_width_chars(75);
	magPopover->add(*magEntry);
	addMagnetButton->set_popover(*magPopover);

	sidebar_scrolledwindow->set_min_content_width(150);

	m_rss2->set_transient_for(*this);

	for(auto tor : Application::getSingleton()->getCore()->getTorrents())
	{
		tor->onStateChanged = [this](int oldstate, std::shared_ptr<gt::Torrent> t){ torrentStateChangedCallback(oldstate, t); };
		m_treeview_torrent->addCell(tor);
	}

	for(auto feedg : Application::getSingleton()->getCore()->m_feeds)
	{
		feedg->onStateChanged     = [this](int oldstate, std::shared_ptr<gt::Feed> f)   { feedStateChangedCallback(oldstate, f); };
		feedg->onNewItemAvailable = [this](const libtorrent::feed_item& fi, std::shared_ptr<gt::Feed> fg){ itemAvailableCallback(fi, fg); };
	}

	if (gt::Settings::settings["FileAssociation"] == "" ||
		gt::Settings::settings["FileAssociation"] == "-1")
	{
		GtkAssociationDialog *dialog = 0;
		builder->get_widget_derived("fileAssociationDialog", dialog);
		dialog->set_transient_for(*this);
		dialog->set_default_response(1);
		int code = dialog->run();
		if(code != -1)
			gt::Platform::associate(dialog->aWithMagnets, dialog->aWithTorrents);
		gt::Settings::settings["FileAssociation"] = std::to_string(code);
		delete dialog;
	}

	d = new GtkSettingsDialog(this);
}

/**
* Update interface
* Updates torrent cells
* Hide vertical scrollbar
*/
bool GtkMainWindow::onSecTick()
{
        // Update the gooey
	m_treeview_torrent->updateCells();
	m_infobar->updateState(m_treeview_torrent->getFirstSelected());
        m_sidebar->updateTorrents();

        // Handle new torrents... wait, why the fuck do we handle only one at a time?
        // TODO FIXME XXX
	std::shared_ptr<gt::Torrent> t = m_core->update();
	if (t)
	{
		t->onStateChanged = [this](int oldstate, std::shared_ptr<gt::Torrent> t){ torrentStateChangedCallback(oldstate, t); };
		m_treeview_torrent->addCell(t);
	}

	scrolledWindow->get_vscrollbar()->set_child_visible(false); // WTF is this doing here?
	return true;
}

/**
* Open a FileChooserDialog that filters for bittorent files
*/
void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse for torrent file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_default_size(256, 256);
	fc.set_select_multiple();
	fc.set_transient_for(*this);
	fc.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
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
				t->onStateChanged = [this](int oldstate, std::shared_ptr<gt::Torrent> t){ torrentStateChangedCallback(oldstate, t); };
				m_treeview_torrent->addCell(t);
			}
			//TODO Add error dialogue if torrent add is unsuccessful
		}
		break;
	}
}

void GtkMainWindow::torrentStateChangedCallback(int oldstate, std::shared_ptr<gt::Torrent> t)
{
	NotifyNotification *tNotify = nullptr;

	int newstate = t->status().state;

	if(newstate == libtorrent::torrent_status::seeding && oldstate == libtorrent::torrent_status::downloading)
		tNotify = notify_notification_new (t->status().name.c_str(), std::string(t->status().name + " has finished downloading.").c_str(), "dialog-information");
	else if(newstate == libtorrent::torrent_status::downloading  &&
			oldstate == libtorrent::torrent_status::downloading_metadata)
		tNotify = notify_notification_new (t->status().name.c_str(), std::string(t->status().name + " has started downloading.").c_str(), "dialog-information");
	else
		return; //:^)

	notify_notification_show (tNotify, NULL);
	g_object_unref(G_OBJECT(tNotify));
}

/**
* Display magPopover
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
			m_treeview_torrent->addCell(t);
		}
		magEntry->set_text("");
	}
}

void GtkMainWindow::onPauseBtnClicked()
{
	m_treeview_torrent->setSelectedPaused(true);
	m_treeview_torrent->onSelectionChanged();
}

void GtkMainWindow::onResumeBtnClicked()
{
	m_treeview_torrent->setSelectedPaused(false);
	m_treeview_torrent->onSelectionChanged();
}

void GtkMainWindow::onRemoveBtnClicked()
{
	for(auto t : m_treeview_torrent->selectedTorrents())
		m_infobar->removeInfo(t);
	m_treeview_torrent->removeSelected();
}

void GtkMainWindow::onPropertiesBtnClicked()
{

}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_treeview_torrent->saveColumns();
	notify_uninit();
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
		m_treeview_torrent->m_searchPopover->set_visible();
		if(std::find(enter, enter + 2, event->hardware_keycode) != enter + 2)
			m_treeview_torrent->m_searchPopover->set_visible(false);
		m_treeview_torrent->m_filter->refilter();
	}
	return false;
}

void GtkMainWindow::feedStateChangedCallback(int oldstate, std::shared_ptr<gt::Feed> fg)
{
// TODO: if user want to be notified, if the item passes a filter of any of its owner, show a notification, and
// if the item passes all the filters of an owner that wants auto-adding, add it here.
}

void GtkMainWindow::itemAvailableCallback(const libtorrent::feed_item& fi, std::shared_ptr<gt::Feed> fg)
{
	bool notify = gt::Settings::settings["RSSNotify"] == "Yes";
	for(auto group : fg->owners)
		if(group->passFilters(fi))
		{
			if(notify)
			{
				NotifyNotification *rssNotify = notify_notification_new ("New torrent available", std::string(fi.title + " is available.").c_str(), "dialog-information");
				notify_notification_show(rssNotify, nullptr);
				g_object_unref(G_OBJECT(rssNotify));
			}
			if(group->autoAddNewItem)
			{
				auto tor = m_core->addTorrent(fi.url);
				m_treeview_torrent->addCell(tor);
			}
		}
// TODO: if user want to be notified, if the item passes a filter of any of its owner, show a notification, and
// if the item passes all the filters of an owner that wants auto-adding, add it here.
}
