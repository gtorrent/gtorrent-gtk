#include "Gtk.hpp"
#include "GtkMainWindow.hpp"

/**
* Sets up the main window.
*/
GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);

	GtkMainWindow mainWindow;

	string binpath = argv[0];
	binpath = binpath.substr(0, binpath.find_last_of(gt::Platform::getFileSeparator()));
	mainWindow.set_icon_from_file(binpath + gt::Platform::getFileSeparator() + "gtorrent.png");

	kit.run(mainWindow);
}
