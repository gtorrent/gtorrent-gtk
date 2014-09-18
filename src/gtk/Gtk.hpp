#pragma once

// 
#include <gtkmm.h>
#include <glibmm.h>
#include "../core/Base.hpp"

namespace gt
{
	class GuiGtk : public GuiBase
	{
		Glib::RefPtr<Gtk::Application> m_app;
		Glib::RefPtr<Gtk::Builder> m_builder;
	public:
		GuiGtk();
		virtual int run(int argc, char **argv);
	};
}
