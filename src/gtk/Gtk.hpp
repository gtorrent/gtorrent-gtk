#pragma once

#include <gtkmm/settings.h>
#include <core/Base.hpp>
#include <gtkmm/main.h>

#include "GtkMainWindow.hpp"

class GuiGtk : public GuiBase
{
public:
	GuiGtk(int argc, char **argv);
};
