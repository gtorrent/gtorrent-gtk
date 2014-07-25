#include "Gtk.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);

	GtkMainWindow mainWindow;
	kit.run(mainWindow);
}
