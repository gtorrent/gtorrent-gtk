#include "GTorrentApp.hpp"
#include <iostream>

GTorrentApp::GTorrentApp() : Gtk::Application("com.gtorrent.gt-gtk", Gio::APPLICATION_HANDLES_OPEN)
{
}

std::shared_ptr<GTorrentApp> GTorrentApp::get()
{
    static auto singleton = std::make_shared<GTorrentApp>();
    return singleton;
}

void GTorrentApp::on_startup()
{
    Gtk::Application::on_startup();
    Glib::set_application_name("gTorrent");
    std::cout << "startup signal\n";
}

void GTorrentApp::on_activate()
{
    std::cout << "activate signal\n";
 
    auto window = new Gtk::Window;
    add_window(*window);
    window->show();
}

void GTorrentApp::on_open(const std::vector<Glib::RefPtr<Gio::File>>& files, const Glib::ustring& hint)
{
    for (auto& f : files)
        std::cout << "opening " << f->get_path() << "\n";
}

int main(int argc, char *argv[])
{
    return GTorrentApp::get()->run(argc, argv);
}
