#pragma once

#include <gtorrent/Torrent.hpp>

#include <gtkmm/liststore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/popover.h>

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
		add(m_col_age);
		add(m_col_eta);
		add(m_col_bage);
		add(m_col_beta);
		add(m_col_seeders);
		add(m_col_leechers);
		add(m_col_ul_speed);
		add(m_col_dl_speed);
		add(m_col_bul_speed);
		add(m_col_bdl_speed);
		add(m_col_percent);
		add(m_col_percent_text);
		add(m_col_ul_total);
		add(m_col_dl_total);
		add(m_col_bul_total);
		add(m_col_bdl_total);
		add(m_col_size);
		add(m_col_remaining);
		add(m_col_bsize);
		add(m_col_bremaining);
		add(m_col_dl_ratio);
		add(m_col_background);
		add(m_col_foreground);
		add(m_col_torrent);
	}

	Gtk::TreeModelColumn<unsigned int>  m_col_queue;
	Gtk::TreeModelColumn<unsigned int>  m_col_seeders;
	Gtk::TreeModelColumn<unsigned int>  m_col_leechers;
	Gtk::TreeModelColumn<unsigned int>  m_col_percent;
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

	// Data
	Gtk::TreeModelColumn<unsigned int>  m_col_bage;
	Gtk::TreeModelColumn<unsigned int>  m_col_beta;
	Gtk::TreeModelColumn<unsigned int>  m_col_bul_speed;
	Gtk::TreeModelColumn<unsigned int>  m_col_bdl_speed;
	Gtk::TreeModelColumn<unsigned int>  m_col_bul_total;
	Gtk::TreeModelColumn<unsigned int>  m_col_bdl_total;
	Gtk::TreeModelColumn<unsigned int>  m_col_bsize;
	Gtk::TreeModelColumn<unsigned int>  m_col_bremaining;
	Gtk::TreeModelColumn<Glib::ustring> m_col_background;
	Gtk::TreeModelColumn<Glib::ustring> m_col_foreground;
	Gtk::TreeModelColumn<std::shared_ptr<gt::Torrent>> m_col_torrent;
};

// Gtk Torrent Tree View Section
class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	Gtk::Entry *m_searchEntry;

	Glib::RefPtr<Gtk::ListStore> m_liststore;
	Gtk::Menu *m_rcMenu = Gtk::manage(new Gtk::Menu());
	Gtk::CheckMenuItem *rcmItemSeq;
	std::map<std::string, std::pair<std::string, std::string>> m_colors; // Associates a state with a background and foreground color.

	void setupColumns();

	/* Event handlers for clicks on the controls */
	bool                 showMatches(const Gtk::TreeModel::const_iterator& iter);
	bool                  onKeyPress(GdkEventKey *event);
	bool         torrentView_onClick(GdkEventButton *event);
	bool      torrentColumns_onClick(GdkEventButton *event);
	bool ColumnContextMenu_onRelease(GdkEventButton *event, Gtk::TreeViewColumn *tvc);

	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

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
	GtkTorrentTreeView(GtkTreeView *treeview, const Glib::RefPtr<Gtk::Builder> rbuilder);
	GtkTorrentInfoBar *m_infobar;
	GtkMainWindow *m_parent;

	GtkTorrentTreeView(GtkMainWindow *Parent, GtkTorrentInfoBar *InfoBar);
	Glib::RefPtr<Gtk::TreeModelFilter> m_filter;
	Glib::RefPtr<Gtk::TreeModelSort> m_filtersort;
	Gtk::Popover *m_searchPopover;

	void addCell(std::shared_ptr<gt::Torrent> &t);
	void removeCell(unsigned index);
	void updateCells();
	void setSelectedPaused(bool isPaused);
	void removeSelected();
	void reloadColors();
	void onSelectionChanged();
	std::shared_ptr<gt::Torrent> getFirstSelected();
	std::vector<std::shared_ptr<gt::Torrent>> selectedTorrents();
	void loadColumns();
	void saveColumns();
};
