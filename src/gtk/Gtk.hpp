#pragma once

#include <gtkmm.h>
#include <glibmm.h>

#include "../base/GuiBase.hpp"

namespace gt
{
    // TODO Rename this to gt::Gui and replace the GuiBase class (Or move it to a neutral gt-core class)
	class GuiGtk : public GuiBase
	{
		Glib::RefPtr<Gtk::Application> m_app;
		Glib::RefPtr<Gtk::Builder> m_builder;
	public:
		GuiGtk(int argc, char **argv);
		virtual int run();
	};
}
