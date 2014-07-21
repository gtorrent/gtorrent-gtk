#pragma once

#include <core/Base.hpp>
#include "GtkMainWindow.hpp"
#include <gtkmm/settings.h>
#include <gtkmm/main.h>

class GuiGtk : public GuiBase
{
private:
	GtkMainWindow *mainWindow;
public:
	GuiGtk(int argc, char **argv);
	~GuiGtk();
};