#pragma once

#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <core/Torrent.hpp>

// Gtk Torrent Columns Section

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns()
	{
		add(m_col_name);
		add(m_col_queue);
		add(m_col_status);
		add(m_col_age);
		add(m_col_eta);
		add(m_col_seeders);
		add(m_col_leechers);
		add(m_col_ul_speed);
		add(m_col_dl_speed);
		add(m_col_percent);
		add(m_col_percent_text);
		add(m_col_ul_total);
		add(m_col_dl_total);
		add(m_col_size);
		add(m_col_remaining);
		add(m_col_dl_ratio);
	}

	Gtk::TreeModelColumn<unsigned int>  m_col_queue;
	Gtk::TreeModelColumn<Glib::ustring> m_col_status;
	Gtk::TreeModelColumn<Glib::ustring> m_col_age;
	Gtk::TreeModelColumn<Glib::ustring> m_col_eta;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int>  m_col_seeders;
	Gtk::TreeModelColumn<unsigned int>  m_col_leechers;
	Gtk::TreeModelColumn<unsigned int>  m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<unsigned int>  m_col_empty;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_size;
	Gtk::TreeModelColumn<Glib::ustring> m_col_remaining;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_ratio;
};

// Gtk Torrent Tree View Section

class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;

	Glib::RefPtr<Gtk::ListStore> m_liststore;

	Gtk::Menu *m_rcMenu = Gtk::manage(new Gtk::Menu());

	void setupColumns();
	vector<unsigned> selectedIndices();

	/* Event handlers for clicks on the controls */
	bool       torrentView_onClick(GdkEventButton *event);
	bool    torrentColumns_onClick(GdkEventButton *event);
	bool ColumnContextMenu_onClick(GdkEventButton *event, Gtk::TreeViewColumn *tvc);

	/* Event handlers for the torrent view context menu */
	void     stopView_onClick();
	void     openView_onClick();
	void    startView_onClick();
	void   removeView_onClick();
	void priorityView_onClick();
	void propertyView_onClick();

public:
	GtkTorrentTreeView();

	void addCell(shared_ptr<Torrent> &t);
	void updateCells();
	void setSelectedPaused(bool isPaused);
};
