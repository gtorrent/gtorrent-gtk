#pragma once

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/table.h>

#include <gtorrent/Torrent.hpp>

#include "../Application.hpp"
#include "GtkBlockBar.hpp"
#include "GtkGraph.hpp"

class GtkTorrentInfoBar : public Gtk::Box
{
private:
	GtkBlockBar *m_progress;
	Gtk::Label *m_title;
	GtkGraph *m_graph;
	Gtk::Notebook *m_notebook;
	Gtk::Box *m_stat_box;
	Gtk::Box *m_piece_box;

	Gtk::Table *m_table_layout;
	Gtk::Label *m_down_total_label;
	Gtk::Label *m_down_total;
	Gtk::Label *m_up_total_label;
	Gtk::Label *m_up_total;
public:
	GtkTorrentInfoBar();
	void updateInfo(shared_ptr<gt::Torrent> selected);
};
