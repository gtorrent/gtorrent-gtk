#include <gtkmm.h>
#include <glibmm.h>

#include <gtorrent/Platform.hpp>

#include <exception>
#include <stdexcept>
#include <sys/stat.h>


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
	kit = new Gtk::Main(argc, argv);
	refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_resource("/org/gtk/gtorrent/mainwindow.ui");
		refBuilder->add_from_resource("/org/gtk/gtorrent/association.ui");
		refBuilder->add_from_resource("/org/gtk/gtorrent/infobar.ui");
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
}

extern unsigned char style_css[];

int GuiGtk::run()
{
	try
	{
		GtkMainWindow *mainWindow = 0;
		refBuilder->get_widget_derived("GtkMainWindow", mainWindow);
		mainWindow->set_icon(Gdk::Pixbuf::create_from_resource("/org/gtk/gtorrent/gtorrent.png"));

		auto css = Gtk::CssProvider::create();
		css->load_from_data(std::string((char*)style_css));
		auto screen = Gdk::Screen::get_default();
		Gtk::StyleContext::add_provider_for_screen(screen, css, 800); //Gtk::STYLE_PROVIDER_PRIORITY_APPLICATION); <= compiler can't find it's definition so i'm using the literal value

		
		std::cout << std::string((char*)style_css) << std::endl;
		kit->run(*mainWindow);
	}
	catch(const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
	}
	catch(const Glib::Error& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
	}
	catch(const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
	}



	return 1;
}
