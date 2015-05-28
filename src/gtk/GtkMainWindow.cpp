#include "../Application.hpp"
#include "GtkAssociationDialog.hpp"
#include "GtkMainWindow.hpp"
#include "GtkSettingsDialog.hpp"
#include "torrent/GtkTorrentInfoBar.hpp"
#include "GtkSideBar.hpp"
#include "torrent/GtkTorrentTreeView.hpp"

#include <gtorrent/Core.hpp>
#include <gtorrent/Platform.hpp>
#include <gtorrent/Settings.hpp>

#include <future>
#include <boost/algorithm/string.hpp>

#include <libnotify/notify.h>

#include <giomm.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/main.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrollbar.h>

// TODO: GtkRevealer should contain a listbox, list view of even a tree view like nautilus's
//       so remove the header, customise the cells, and enable the revealer with the property button
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
	// TODO Move this to GtkApplication
	notify_init("gTorrent");

	// Show all children first so widgets that should be hidden can hide.
	show_all_children();

	settingsDialog = new GtkSettingsDialog(this);

	// Headerbar widgets
	builder->get_widget("addTorrentButton", addTorrentButton);
	builder->get_widget("addMagnetButton", addMagnetButton);
	builder->get_widget("addButtonRss", addRssButton);
	builder->get_widget("resumeButton", resumeButton);
	builder->get_widget("pauseButton", pauseButton);
	builder->get_widget("deleteButton", removeButton);
	builder->get_widget("preferencesButton", propertiesButton);
	builder->get_widget("settingsButton", settingsButton);
	builder->get_widget("searchButton", m_searchButton);
	builder->get_widget("vSepOne", vSeparatorOne);
	builder->get_widget("vSepTwo", vSeparatorTwo);

	// Sidebar
	builder->get_widget("sidebarRev", revealer);
	builder->get_widget("sidebarscroll", sidebar_scrolledwindow);
	builder->get_widget_derived("sidebar", m_sidebar);

	// Content
	builder->get_widget("scrolledWindow", scrolledWindow);
	builder->get_widget("content_stack", m_content_stack);
	builder->get_widget_derived("rss_box", m_box_rss);
	builder->get_widget_derived("torrent_box", m_box_torrent);

	m_content_stack->add(*m_box_torrent);
	m_content_stack->add(*m_box_rss);
	m_content_stack->show_all_children();

	// Apparently can't use lambdas on these two unless doing something awful
	Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 1);

	// Set up headerbar buttons
	addTorrentButton->signal_clicked().connect([this](){onClickAdd();});
	addMagnetButton ->signal_clicked().connect([this](){onClickMagnet();});
	addRssButton    ->signal_clicked().connect([this](){onClickRss();});
	settingsButton  ->signal_clicked().connect([this](){onClickSettings();});
	propertiesButton->signal_clicked().connect([this](){revealer->set_reveal_child(!revealer->get_reveal_child());});

	pauseButton     ->signal_clicked().connect([this](){m_box_torrent->onClickPause();});
	resumeButton    ->signal_clicked().connect([this](){m_box_torrent->onClickResume();});
	removeButton    ->signal_clicked().connect([this](){m_box_torrent->onClickRemove();});
	// TODO RSS support
	m_searchButton  ->signal_clicked().connect([this](){m_box_torrent->searchToggle();});

	// Headerbar popovers
	magPopover = Gtk::manage(new Gtk::Popover());
	magPopoverOn = false;
	rssPopover = Gtk::manage(new Gtk::Popover());
	rssPopoverOn = false;
	createPopover(addMagnetButton, magPopover, &magEntry);
	createPopover(addRssButton, rssPopover, &rssEntry);

	sidebar_scrolledwindow->set_min_content_width(150);
	scrolledWindow->get_vscrollbar()->set_child_visible(false);

	for(auto tor : m_core->getTorrents())
		m_box_torrent->torrentAdd(tor);

	for(auto feed : m_core->m_feeds)
	{
		feed->onStateChanged     = [this](int oldstate, gt::Feed* f) {
		    onRssStateChange(oldstate, f); };
		feed->onNewItemAvailable = [this](const libtorrent::feed_item& fi, std::shared_ptr<gt::Feed> fg){
		    onRssItemAvailable(fi, fg); };
	}
}

/**
 * Update interface
 * Updates torrent cells
 * Hide vertical scrollbar
 *
 * TODO Deprecate onSecTick, in favour of polling
 * Don't update by timer, rather, update the view after libtorrent::session informs of one, with a timeout.
 * Use session::wait_for_alert()
 * Similar but some things can override it, for example.
 */
bool GtkMainWindow::onSecTick()
{
	m_core->update();
	std::vector<std::shared_ptr<gt::Torrent>> pending_torrents = m_core->getPendingTorrents();
	for (auto t : pending_torrents)
		if (t) torrentAdd(t);

	// TODO Should do this only if visible
	m_box_torrent->updateTorrents();
	m_sidebar->updateTorrents();

	return true;
}

/**
* Open a FileChooserDialog that filters for bittorrent files
*/
void GtkMainWindow::onClickAdd()
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

	switch (result) {
	case Gtk::RESPONSE_OK:
		for (auto & f : fc.get_filenames()) {
			torrentAdd(f);
		}
		break;
	}
}

/**
* Toggle magPopover
*/
void GtkMainWindow::onClickMagnet()
{
	magPopoverOn = !magPopoverOn;
	if(magPopoverOn) {
		// Popover has been toggled
		fillEntryWithLink(magEntry);
	} else {
		// Popover has been detoggled
		torrentAdd(magEntry->get_text());
		magEntry->set_text("");
	}
}

void GtkMainWindow::onClickRss()
{
	rssPopoverOn = !rssPopoverOn;
	if(rssPopoverOn) {
		// Popover has been toggled
		fillEntryWithLink(rssEntry);
	} else {
		// Popover has been detoggled
		feedAdd(rssEntry->get_text());
		rssEntry->set_text("");
	}
}

void GtkMainWindow::onClickProperties()
{

}

void GtkMainWindow::onClickSettings()
{
	settingsDialog->run();
}

/**
 * Used for input on searchbar
 * TODO Since searchbar has been moved to SearchBox so should this
 */
bool GtkMainWindow::onKeyPress(GdkEventKey *event)
{
//	short arrowkeys[] = { 80, 88, 83, 85, 111, 114, 113, 116 };
//	short enter[] = { 36, 104 };
//	if(std::find(arrowkeys, arrowkeys + 8, event->hardware_keycode) == arrowkeys + 8)
//	{
//		m_treeview_torrent->m_searchPopover->set_visible();
//		if(std::find(enter, enter + 2, event->hardware_keycode) != enter + 2)
//			m_treeview_torrent->m_searchPopover->set_visible(false);
//		m_treeview_torrent->m_filter->refilter();
//	}
	return false;
}

void GtkMainWindow::onRssStateChange(int oldstate, gt::Feed* fg)
{
// TODO: if user want to be notified, if the item passes a filter of any of its owner, show a notification, and
// if the item passes all the filters of an owner that wants auto-adding, add it here.
}

void GtkMainWindow::onRssItemAvailable(const libtorrent::feed_item &fi, std::shared_ptr<gt::Feed> f)
{
	bool notify = gt::Settings::settings["RSSNotify"] == "Yes";
//	if(group->passFilters(fi))
//	{
	if(notify)
	{
		NotifyNotification *rssNotify = notify_notification_new ("Rss update", std::string(fi.title).c_str(), "dialog-information");
		notify_notification_show(rssNotify, nullptr);
		g_object_unref(G_OBJECT(rssNotify));
	}
	// Shouldn't be needed if libtorrent::feed can add torrents automatically by themselves
//	if(f->autoAddNewItem)
//		torrentAdd(fi.url);
	// TODO: if user want to be notified, if the item passes a filter of any of its owner, show a notification, and
	// if the item passes all the filters of an owner that wants auto-adding, add it here.
}

/**
 * Populates entry with text from the X clipboard if the X clipboard contains a link
 * This function can be made standalone as a utility function
 */
void GtkMainWindow::fillEntryWithLink(Gtk::Entry *entry)
{
	Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
	std::string link = clip->wait_for_text();
	if(gt::Core::isLink(link))
		entry->set_text(link);
}

/**
 * Sets popover to MenuButton
 * Probably not worth being set into its own function
 * Should be renamed or inlined again.
 */
void GtkMainWindow::createPopover(Gtk::MenuButton *b, Gtk::Popover *p, Gtk::Entry **e)
{
	p->set_relative_to(*b);
	p->set_position(Gtk::POS_LEFT);
	*e = Gtk::manage(new Gtk::Entry());
	(*e)->set_visible();
	(*e)->set_width_chars(75);
	p->add(**e);
	b->set_popover(*p);
}

/**
 * Deprecated legacy function that should be removed once file association is done on install
 * Here for prosterity
 */
void GtkMainWindow::showAssociationDialog()
{
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

}

void GtkMainWindow::torrentAdd(std::shared_ptr<gt::Torrent> t)
{
	m_box_torrent->torrentAdd(t);
}

void GtkMainWindow::torrentAdd(const std::string &f)
{
	auto tor = m_core->addTorrent(f);
	if (tor)
		torrentAdd(tor);
}

void GtkMainWindow::feedAdd(std::string feed)
{
	auto fp = m_core->addFeed(feed);
	if (fp) {
//		fp->onStateChanged = std::bind(&GtkMainWindow::onRssStateChange, this, std::placeholders::_1, std::placeholders::_2);
//		fp->onNewItemAvailable = std::bind(&GtkMainWindow::onRssItemAvailable, this, std::placeholders::_1, std::placeholders::_2);
		fp->onStateChanged = [this](int oldstate, gt::Feed* f) { onRssStateChange(oldstate, f); };
		fp->onNewItemAvailable = [this](const libtorrent::feed_item& fi, std::shared_ptr<gt::Feed> fg){
		    onRssItemAvailable(fi, fg); };
		m_box_rss->feedAdd(fp);
	}
}
