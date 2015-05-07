#pragma once

#include "../GtkSettingsDialog.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkTorrentTreeView.hpp"

#include <gtorrent/Core.hpp>

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/paned.h>
#include <gtkmm/searchbar.h>
#include <gtkmm/stack.h>

class GtkTorrentBox : public Gtk::Box
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;

public:
	GtkTorrentBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder);

	GtkTorrentInfoBar   *m_infobar;
	GtkTorrentTreeView  *m_treeview_torrent;

	Gtk::Paned          *m_panel;
	Gtk::Box            *m_torrentbox;
	Gtk::SearchBar      *m_searchbar;
	Gtk::Stack          *m_stack_info;
	Gtk::Stack          *m_stack_main;

	void torrentAdd(std::shared_ptr<gt::Torrent> shared_ptr);
	void onClickPause();
	void onClickRemove();
	void onClickResume();
	void searchToggle();
	void updateTorrents();

	void saveColumns();

	void onTorrentStateChange(int oldstate, std::shared_ptr <gt::Torrent> t);
};
