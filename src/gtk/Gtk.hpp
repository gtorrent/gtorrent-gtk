#pragma once

#include <gtkmm/settings.h>
#include <gtkmm/main.h>

#include "../core/Base.hpp"

class GuiGtk : public GuiBase
{
public:
	GuiGtk(int argc, char **argv);
};
