#pragma once

#include <gtorrent/Torrent.hpp>

#include <gtkmm/liststore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>

namespace Gtk
{
	class CellRendererProgress;
	class CheckMenuItem;
	class MenuItem;
}

class GtkMainWindow;
class GtkTorrentInfoBar;

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns()
	{
		add(m_col_name);
		add(m_col_queue);
		add(m_col_size);
		add(m_col_selected_size);
		add(m_col_completed);
		add(m_col_downloaded);
		add(m_col_remaining);
		add(m_col_done);
		add(m_col_status);
		add(m_col_seeds);
		add(m_col_peers);
		add(m_col_seeds_peers);
		add(m_col_dl_speed);
		add(m_col_ul_speed);
		add(m_col_eta);
		add(m_col_uploaded);
		add(m_col_dl_ratio);

		add(m_col_avail);
		add(m_col_label);
		add(m_col_added_on);
		add(m_col_completed_on);
		add(m_col_tracker);
		add(m_col_up_limit);
		add(m_col_down_limit);
		add(m_col_bandwidth_allocation);
		add(m_col_tracker_status);
		add(m_col_debug);
		add(m_col_last_active);
		add(m_col_age);
		add(m_col_source_url);
		add(m_col_episode);
		add(m_col_format);
		add(m_col_codec);

		add(m_col_percent);
		add(m_col_percent_text);
		add(m_col_ul_total);
		add(m_col_dl_total);

		add(m_col_background);
		add(m_col_foreground);
		add(m_col_torrent);
	}

Gtk::TreeModelColumn<Glib::ustring>           m_col_name;

	Gtk::TreeModelColumn<unsigned int>            m_col_queue;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_size;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_selected_size;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_completed;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_downloaded;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_remaining;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_done;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_status;
	Gtk::TreeModelColumn<unsigned int>            m_col_seeds;
	Gtk::TreeModelColumn<unsigned int>            m_col_peers;
	Gtk::TreeModelColumn<unsigned int>            m_col_seeds_peers;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_dl_speed;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_ul_speed;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_eta;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_uploaded;

Gtk::TreeModelColumn<Glib::ustring>           m_col_avail;
Gtk::TreeModelColumn<Glib::ustring>           m_col_label;
Gtk::TreeModelColumn<Glib::ustring>           m_col_added_on;
Gtk::TreeModelColumn<Glib::ustring>           m_col_completed_on;
Gtk::TreeModelColumn<Glib::ustring>           m_col_tracker;
Gtk::TreeModelColumn<Glib::ustring>           m_col_up_limit;
Gtk::TreeModelColumn<Glib::ustring>           m_col_down_limit;
Gtk::TreeModelColumn<Glib::ustring>           m_col_bandwidth_allocation;
Gtk::TreeModelColumn<Glib::ustring>           m_col_tracker_status;
Gtk::TreeModelColumn<Glib::ustring>           m_col_debug;
Gtk::TreeModelColumn<Glib::ustring>           m_col_last_active;
Gtk::TreeModelColumn<Glib::ustring>           m_col_age;
Gtk::TreeModelColumn<Glib::ustring>           m_col_source_url;
Gtk::TreeModelColumn<Glib::ustring>           m_col_episode;
Gtk::TreeModelColumn<Glib::ustring>           m_col_format;
Gtk::TreeModelColumn<Glib::ustring>           m_col_codec;

	Gtk::TreeModelColumn<unsigned int>            m_col_percent;
	Gtk::TreeModelColumn<unsigned int>            m_col_empty;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_percent_text;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_dl_ratio;

	Gtk::TreeModelColumn<Glib::ustring>           m_col_ul_total;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_dl_total;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_background;
	Gtk::TreeModelColumn<Glib::ustring>           m_col_foreground;
	Gtk::TreeModelColumn<std::shared_ptr<gt::Torrent>> m_col_torrent;
};

// Gtk Torrent Tree View Section
class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	GtkTorrentInfoBar *m_infobar;
	GtkMainWindow *m_parent;

	Glib::RefPtr<Gtk::ListStore> m_liststore;
	Gtk::Menu *m_rcMenu = Gtk::manage(new Gtk::Menu());
	Gtk::CheckMenuItem *rcmItemSeq;
	std::map<std::string, std::pair<std::string, std::string>> m_colors; // Associates a state with a background and foreground color.

	void setupColumns();
	std::vector<std::shared_ptr<gt::Torrent>> selectedTorrents();

	/* Event handlers for clicks on the controls */
	bool				  onKeyPress(GdkEventKey *event);
	bool         torrentView_onClick(GdkEventButton *event);
	bool      torrentColumns_onClick(GdkEventButton *event);
	bool ColumnContextMenu_onRelease(GdkEventButton *event, Gtk::TreeViewColumn *tvc);

	/* Event handlers for the torrent view context menu */
	void           stopView_onClick();
	void           openView_onClick();
	void          startView_onClick();
	void         removeView_onClick();
	void       priorityView_onClick();
	void       propertyView_onClick();
	void   sequentialChange_onClick();
	void sequentialChange_onRealize();

public:
	GtkTorrentTreeView(GtkMainWindow *Parent, GtkTorrentInfoBar *InfoBar);

	void addCell(std::shared_ptr<gt::Torrent> &t);
	void removeCell(unsigned index);
	void updateCells();
	void setSelectedPaused(bool isPaused);
	void removeSelected();
	void reloadColors();
	void onSelectionChanged();
	std::shared_ptr<gt::Torrent> getFirstSelected();
	void loadColumns();
	void saveColumns();
};
