#pragma once

#include <gtkmm.h>
#include <glibmm.h>

#include "../core/GuiBase.hpp"

namespace gt
{
	class GuiGtk : public GuiBase
	{
		Glib::RefPtr<Gtk::Application> m_app;
		Glib::RefPtr<Gtk::Builder> m_builder;
	public:
		GuiGtk(int argc, char **argv);
		virtual int run();
	};
}
