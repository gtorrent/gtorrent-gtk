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
	GtkRssTreeView *m_rss_treeview;
	GtkRssBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder);

	void feedAdd(std::shared_ptr < gt::Feed > param);
};
