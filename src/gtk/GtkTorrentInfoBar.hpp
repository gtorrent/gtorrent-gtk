#pragma once

#include <gtkmm/box.h>
#include <gtorrent/Torrent.hpp>
#include <gtkmm/builder.h>

#include "GtkGraph.hpp"

namespace Gtk
{
	class Notebook;
	class Label;
	class ScrolledWindow; 
}

class GtkBlockBar;
class GtkStatusBox;
class GtkPeerTreeView;
class GtkFileTreeView;

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
	Gtk::ScrolledWindow *m_peer_scroll_box;
	GtkPeerTreeView *m_peers;
	Gtk::ScrolledWindow *m_filebox;
	GtkFileTreeView *m_fileview;

public:
	const Glib::RefPtr<Gtk::Builder> builder;
	GtkTorrentInfoBar(GtkBox *box, const Glib::RefPtr<Gtk::Builder> rbuilder);
	inline void removeInfo (std::shared_ptr<gt::Torrent> selected)
	{
		m_graph->removeHistory(selected);
	}
	void updateInfo (std::shared_ptr<gt::Torrent> selected);
	void updateState(std::shared_ptr<gt::Torrent> selected);
};
