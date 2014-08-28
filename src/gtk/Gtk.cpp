#include <glibmm/fileutils.h>
#include <glibmm/markup.h>
#include <exception>
#include <stdexcept>
#include <sys/stat.h>
#include <gtkmm/builder.h>
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
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtorrent.gtk");
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_resource("/org/gtk/gtorrent/mainwindow.ui");
	}
	catch(const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
		return;
	}
	catch(const Glib::MarkupError& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return ;
	}
	catch(const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return ;
	}

	GtkMainWindow *mainWindow = 0;
	refBuilder->get_widget_derived("GtkMainWindow", mainWindow);
	mainWindow->set_icon(Gdk::Pixbuf::create_from_resource("/org/gtk/gtorrent/gtorrent.png"));
	app->run(*mainWindow, argc, argv);
}
