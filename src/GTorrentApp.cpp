#include "GTorrentApp.hpp"

#include <gtorrent/Log.hpp>

#include "Version.hpp"
#include "DefaultSettings.hpp"
#include "gtk/GtkMainWindow.hpp"

GTorrentApp::GTorrentApp() : Gtk::Application("com.gtorrent.gt-gtk", Gio::APPLICATION_HANDLES_OPEN)
{
}

GTorrentApp::~GTorrentApp()
{
	this->core->shutdown();
}

std::shared_ptr<GTorrentApp> GTorrentApp::getApp()
{
	static auto singleton = std::make_shared<GTorrentApp>();
	return singleton;
}

std::shared_ptr< gt::Core > GTorrentApp::getCore()
{
	return getApp()->core;
}

void GTorrentApp::on_startup()
{
	Gtk::Application::on_startup();
	Glib::set_application_name("gTorrent");

	gt::Log::Debug("gTorrent v" GTORRENT_VERSION);
	gt::Log::Debug("Starting up core library");

	core = std::make_shared<gt::Core>();
	loadDefaultSettings();
}

extern unsigned char style_css[]; // TODO: get rid of this

// TODO: handle multiple activated signals
void GTorrentApp::on_activate()
{
	gt::Log::Debug("Creating GUI");
	auto builder = Gtk::Builder::create();
	try
	{
		builder->add_from_resource("/org/gtk/gtorrent/window_main.ui");
		builder->add_from_resource("/org/gtk/gtorrent/dialog_rss.ui");
		builder->add_from_resource("/org/gtk/gtorrent/dialog_association.ui");
		builder->add_from_resource("/org/gtk/gtorrent/panel_info.ui");

		GtkMainWindow *window = nullptr;
		builder->get_widget_derived("GtkMainWindow", window);
		add_window(*window);
		window->set_icon(Gdk::Pixbuf::create_from_resource("/org/gtk/gtorrent/gtorrent.png"));
		window->show_all();

		auto css = Gtk::CssProvider::create();
		css->load_from_data(std::string((char*)style_css));
		Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}
	catch(const Glib::Error& ex)
	{
		// TODO: use gt::Log
		std::cerr << "Gtk::Builder threw an error: " << ex.what() << std::endl;
		return;
	}
}

void GTorrentApp::on_open(const std::vector<Glib::RefPtr<Gio::File>>& files, const Glib::ustring& hint)
{
	for (auto& f : files)
		std::cout << "opening " << f->get_path() << "\n";
}

int main(int argc, char *argv[])
{
	return GTorrentApp::getApp()->run(argc, argv);
}
