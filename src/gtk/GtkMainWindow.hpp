#pragma once

#include "rss/GtkRSSDialog.hpp"
#include "torrent/GtkTorrentInfoBar.hpp"

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
class GtkTorrentSideBar;
class GtkSettingsDialog;

class GtkMainWindow : public Gtk::Window
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *d = nullptr;

	// Signal Responders
	bool onKeyPress(GdkEventKey *event);

	void onAboutBtnClicked();
	void onClickAdd();
	void onClickMagnet();
	void onClickPause();
	void onClickResume();
	void onClickRemove();
	void onClickSettings();
	void onPropertiesBtnClicked();
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

public:
	void torrentStateChangedCallback(int oldstate, std::shared_ptr<gt::Torrent> t);
	void feedStateChangedCallback(int oldstate, std::shared_ptr<gt::Feed> fg);
	void itemAvailableCallback(const libtorrent::feed_item& fi, std::shared_ptr<gt::Feed> fg);

	GtkTorrentTreeView *m_treeview_torrent;
	GtkTorrentTreeView *m_treeview_rss;
	GtkTorrentInfoBar  *m_infobar;
	GtkTorrentSideBar  *m_sidebar;
	GtkRSSDialog       *m_rss2;

	Gtk::ScrolledWindow *scrolledWindow;
	Gtk::ScrolledWindow *sidebar_scrolledwindow;
	Gtk::Paned          *m_panel;
	Gtk::Box            *m_torrentbox;
	Gtk::SearchBar      *m_searchbar;
	Gtk::Stack          *m_stack_info;
	Gtk::Stack          *m_stack_main;

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

	void onClickRss();

	void fillEntryWithLink(Gtk::Entry * entry);

	void createPopover(Gtk::MenuButton * pButton, Gtk::Popover * pPopover, Gtk::Entry * pEntry);
};
