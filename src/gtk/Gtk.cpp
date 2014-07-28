#include "Gtk.hpp"
#include "GtkMainWindow.hpp"

/**
* Checks if specified file exists
*/
// TODO Use platform-specific code
bool exists (const std::string& name) {
struct stat buffer;
return (stat (name.c_str(), &buffer) == 0);
}

/**
* Sets up the main window.
*/
GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);

	GtkMainWindow mainWindow;

	string binpath = argv[0];
	binpath = binpath.substr(0, binpath.find_last_of("/")); //TODO: create and use Platform::fileSeparator

	string iconpath = binpath + "/gtorrent.png";
	if (exists (iconpath)) {
		mainWindow.set_icon_from_file(iconpath);
	}

	kit.run(mainWindow);
}
