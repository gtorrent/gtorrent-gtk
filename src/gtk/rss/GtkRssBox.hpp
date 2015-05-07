#pragma once

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtorrent/Core.hpp>
#include "GtkRssTreeView.hpp"

class GtkRssBox : public Gtk::Box
{
	const Glib::RefPtr<Gtk::Builder> m_builder;
	std::shared_ptr<gt::Core> &m_core;

public:
	GtkRssBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder);
	GtkRssTreeView *m_treeview_rss;
};
