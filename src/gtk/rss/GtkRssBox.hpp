#pragma once

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtorrent/Core.hpp>

class GtkRssBox : public Gtk::Box
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;

public:
};
