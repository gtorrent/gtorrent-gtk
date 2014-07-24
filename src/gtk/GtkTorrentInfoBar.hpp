#pragma once

#include <gtkmm/box.h>
#include <gtorrent/Torrent.hpp>
#include "GtkBlockBar.hpp"

class GtkTorrentInfoBar : public Gtk::Box
{
private:
	GtkBlockBar *m_progress;
	Gtk::Label *m_title;
public:
	GtkTorrentInfoBar();
	void updateInfo(shared_ptr<gt::Torrent> t);
};
