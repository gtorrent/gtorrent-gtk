#pragma once

#include <gtkmm/box.h>
#include <gtorrent/Torrent.hpp>

namespace Gtk
{
	class Notebook;
	class Label;
	class ScrolledWindow; 
}

class GtkBlockBar;
class GtkGraph;
class GtkStatusBox;
class GtkPeerTreeView;

class GtkTorrentInfoBar : public Gtk::Box
{
private:
	GtkBlockBar   *m_progress;
	Gtk::Label    *m_title;
	GtkGraph      *m_graph;
	Gtk::Notebook *m_notebook;
	Gtk::Box      *m_stat_box;
	Gtk::Box      *m_piece_box;
	Gtk::ScrolledWindow *m_scroll_box;
	GtkStatusBox        *m_status_box;
	GtkPeerTreeView *m_peers;

public:
	GtkTorrentInfoBar();
	void updateInfo (std::shared_ptr<gt::Torrent> selected);
	void updateState(std::shared_ptr<gt::Torrent> selected);
};
