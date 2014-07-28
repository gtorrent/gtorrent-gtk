#pragma once

#include <gtkmm/settings.h>
#include <gtkmm/main.h>

#include "../core/Base.hpp"
#include <gtorrent/Platform.hpp>

class GuiGtk : public GuiBase
{
public:
	GuiGtk(int argc, char **argv);
};
