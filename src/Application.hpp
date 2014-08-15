#pragma once

#include "gtk/Gtk.hpp"
#include "Version.hpp"
#include <gtorrent/Log.hpp>

class Application
{
private:
	static std::shared_ptr<Application> m_app;

	// Private Member Functions & Variables
	std::shared_ptr<gt::Core> m_core;
	std::shared_ptr<GuiBase> m_gui;

public:
	static std::shared_ptr<Application> getSingleton();

	// Getters
	std::shared_ptr<gt::Core> &getCore();

	// Public Member Functions & Variables
	int run(int argc, char **argv);
};
