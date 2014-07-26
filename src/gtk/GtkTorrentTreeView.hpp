#pragma once

#include <gtorrent/Torrent.hpp>

#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/treeviewcolumn.h>

#include "GtkTorrentInfoBar.hpp"

#include <Application.hpp>

// Gtk Torrent Columns Section

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns()
	{
		add(m_col_name);
		add(m_col_queue);
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
		add(m_col_background);
		add(m_col_foreground);
	}

	Gtk::TreeModelColumn<unsigned int>  m_col_queue;
	Gtk::TreeModelColumn<unsigned int>  m_col_seeders;
	Gtk::TreeModelColumn<unsigned int>  m_col_leechers;
	Gtk::TreeModelColumn<unsigned int>  m_col_percent;
	Gtk::TreeModelColumn<unsigned int>  m_col_empty;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<Glib::ustring> m_col_age;
	Gtk::TreeModelColumn<Glib::ustring> m_col_eta;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_size;
	Gtk::TreeModelColumn<Glib::ustring> m_col_remaining;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_ratio;
	Gtk::TreeModelColumn<Glib::ustring> m_col_background;
	Gtk::TreeModelColumn<Glib::ustring> m_col_foreground;
};

// Gtk Torrent Tree View Section
class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	std::map<string, pair<string, string>> m_colors; // Associates a state with a background and foreground color.
	Glib::RefPtr<Gtk::ListStore> m_liststore;
	GtkTorrentInfoBar *m_infobar;
	Gtk::Menu *m_rcMenu = Gtk::manage(new Gtk::Menu());
	Gtk::CheckMenuItem *rcmItemSeq;

	void setupColumns();
	vector<unsigned> selectedIndices();

	/* Event handlers for clicks on the controls */
	bool         torrentView_onClick(GdkEventButton *event);
	bool      torrentColumns_onClick(GdkEventButton *event);
	bool ColumnContextMenu_onRelease(GdkEventButton *event, Gtk::TreeViewColumn *tvc);

	/* Event handlers for the torrent view context menu */
	void         stopView_onClick();
	void         openView_onClick();
	void        startView_onClick();
	void       removeView_onClick();
	void     priorityView_onClick();
	void     propertyView_onClick();
	void  sequentialChange_onRealize();
	void sequentialChange_onClick();

public:
	GtkTorrentTreeView(GtkTorrentInfoBar *InfoBar);

	void addCell(shared_ptr<gt::Torrent> &t);
	void removeCell(unsigned index);
	void updateCells();
	void setSelectedPaused(bool isPaused);
	void removeSelected();
	shared_ptr<gt::Torrent> getFirstSelected();
};
