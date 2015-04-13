#pragma once

#include <vector>
#include <gtkmm.h>

class GTorrentApp : public Gtk::Application {
public:
    GTorrentApp();

    static std::shared_ptr<GTorrentApp> get();

protected:
    virtual void on_startup();
    virtual void on_activate();
    virtual void on_open(const type_vec_files& files, const Glib::ustring& hint);
};