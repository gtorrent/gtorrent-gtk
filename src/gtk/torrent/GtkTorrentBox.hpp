#pragma once

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtorrent/Core.hpp>
#include <gtkmm/paned.h>
#include <gtkmm/searchbar.h>
#include <gtkmm/stack.h>
#include "../GtkSettingsDialog.hpp"

class GtkTorrentBox : Gtk::Box
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *d = nullptr;

public:
	GtkTorrentBox(GtkBox *box, Glib::RefPtr <Gtk::Builder> const &refBuilder);

	Gtk::Paned          *m_panel;
	Gtk::Box            *m_torrentbox;
	Gtk::SearchBar      *m_searchbar;
	Gtk::Stack          *m_stack_info;
	Gtk::Stack          *m_stack_main;
};
