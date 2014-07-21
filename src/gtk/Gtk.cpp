#include "Gtk.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);
	mainWindow = new GtkMainWindow();

	Gtk::Main::run();
}

GuiGtk::~GuiGtk()
{
	// You know, we could use auto and
	// make_shared(...) instead so we still
	// receive benefits of shared_ptr
	// without the xbauxhueg mess

	// This is called after Core::shutdown

	delete mainWindow;
}