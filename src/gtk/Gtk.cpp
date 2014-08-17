#include <sys/stat.h>

#include <gtkmm/settings.h>
#include <gtkmm/main.h>

#include <gtorrent/Platform.hpp>

#include "GtkMainWindow.hpp"
#include "Gtk.hpp"

/**
* Checks if specified file exists
*/
// TODO Use platform-specific code
bool exists (const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

/**
* Sets up the main window.
*/
GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit(argc, argv);

	GtkMainWindow mainWindow;

	std::string binpath = argv[0];
	binpath = binpath.substr(0, binpath.find_last_of(gt::Platform::getFileSeparator()));
	std::string iconpath = binpath + gt::Platform::getFileSeparator() + "gtorrent.png";
	if (exists (iconpath))
		mainWindow.set_icon_from_file(iconpath);

	kit.run(mainWindow);
}
