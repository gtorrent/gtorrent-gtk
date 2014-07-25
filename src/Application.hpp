#pragma once

#include <gtk/Gtk.hpp>
#include <Version.hpp>
#include <gtorrent/Log.hpp>

class Application
{
private:
	static shared_ptr<Application> m_app;

	// Private Member Functions & Variables
	shared_ptr<gt::Core> m_core;
	shared_ptr<GuiBase> m_gui;

public:
	static shared_ptr<Application> getSingleton();

	// Getters
	shared_ptr<gt::Core> &getCore();

	// Public Member Functions & Variables
	int run(int argc, char **argv);
};
