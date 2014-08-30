#pragma once

#include <gtkmm.h>
#include <glibmm.h>
#include "../core/Base.hpp"

class GuiGtk : public GuiBase
{
    Gtk::Main *kit;
	Glib::RefPtr<Gtk::Builder> refBuilder;
public:
	GuiGtk(int argc, char **argv);
	virtual int run();
};
