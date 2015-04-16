#pragma once

#include "rss/GtkRSSDialog.hpp"
#include "torrent/GtkTorrentInfoBar.hpp"
#include "torrent/GtkTorrentBox.hpp"

#include <gtorrent/Core.hpp>
#include <gtorrent/FeedGroup.hpp>

// TODO Just predeclare these instead of including. Only one you need is Window
#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/paned.h>
#include <gtkmm/popover.h>
#include <gtkmm/revealer.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/searchbar.h>
#include <gtkmm/separator.h>
#include <gtkmm/stack.h>
#include <gtkmm/window.h>

class GtkTorrentTreeView;
class GtkSideBar;
class GtkSettingsDialog;

class GtkMainWindow : public Gtk::Window
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *settingsDialog = nullptr;

	// Signal Responders
	bool onKeyPress(GdkEventKey *event);

	void onAboutBtnClicked();
	void onClickAdd();
	void onClickMagnet();
	void onClickPause();
	void onClickProperties();
	void onClickRemove();
	void onClickResume();
	void onClickRss();
	void onClickSettings();
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
	void onPropertiesBtnClicked();

	void createPopover(Gtk::MenuButton *b, Gtk::Popover *p, Gtk::Entry **e);
	void fillEntryWithLink(Gtk::Entry * entry);

public:
	Gtk::Revealer      *revealer;
	GtkSideBar         *m_sidebar;
	GtkTorrentTreeView *m_treeview_rss;
	GtkTorrentTreeView *m_treeview_torrent;
	GtkTorrentBox      *m_box_torrent;
	GtkRSSDialog       *m_rss2;

	Gtk::Stack          *content_stack;
	Gtk::ScrolledWindow *scrolledWindow;
	Gtk::ScrolledWindow *sidebar_scrolledwindow;

	Gtk::Button         *addTorrentButton;
	Gtk::MenuButton     *addMagnetButton;
	Gtk::Button         *resumeButton;
	Gtk::Button         *pauseButton;
	Gtk::Button         *removeButton;
	Gtk::MenuButton     *buttonRss;
	Gtk::Button         *propertiesButton;
	Gtk::Button         *settingsButton;
	Gtk::Button         *m_searchButton;
	Gtk::Separator      *vSeparatorOne;
	Gtk::Separator      *vSeparatorTwo;
	Gtk::Popover        *magPopover;
	Gtk::Entry          *magEntry;
	Gtk::Popover        *rssPopover;
	Gtk::Entry          *rssEntry;

	GtkMainWindow(GtkWindow*, const Glib::RefPtr<Gtk::Builder>);
	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
	void onRssItemAvailable(const libtorrent::feed_item &fi, std::shared_ptr<gt::Feed> fg);
	void onRssStateChange(int oldstate, std::shared_ptr<gt::Feed> fg);
	void showAssociationDialog();
	void torrentAdd(std::shared_ptr<gt::Torrent>);
	void torrentAdd(const std::string &f);
};
