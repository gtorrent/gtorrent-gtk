#pragma once

#include <gtkmm/box.h>
#include <gtorrent/Torrent.hpp>
#include "GtkBlockBar.hpp"
#include "GtkGraph.hpp"

class GtkTorrentInfoBar : public Gtk::Box
{
private:
	GtkBlockBar *m_progress;
	Gtk::Label *m_title;
	GtkGraph *m_graph;
public:
	GtkTorrentInfoBar();
	void updateInfo(shared_ptr<gt::Torrent> selected);
};
