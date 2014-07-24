#pragma once

#include <gtkmm/infobar.h>
#include <gtkmm/entry.h>
#include <gtorrent/Torrent.hpp>
#include "GtkBlockBar.hpp"

class GtkTorrentInfoBar : public Gtk::Box
{
private:
    GtkBlockBar *m_progress;
    Gtk::Entry *m_title;
public:
    GtkTorrentInfoBar();
    void updateInfo(shared_ptr<gt::Torrent> t);
};
