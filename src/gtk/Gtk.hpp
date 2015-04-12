#pragma once

#include <gtkmm.h>
#include <glibmm.h>

#include "../core/GuiBase.hpp"

namespace gt
{
    // TODO Rename this to gt::Gui and replace the GuiBase class
	class GuiGtk : public GuiBase
	{
		Glib::RefPtr<Gtk::Application> m_app;
		Glib::RefPtr<Gtk::Builder> m_builder;
	public:
		GuiGtk(int argc, char **argv);
		virtual int run();
	};
}
