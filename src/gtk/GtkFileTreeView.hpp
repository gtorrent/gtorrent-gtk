#include <gtorrent/Torrent.hpp>

#include <gtkmm/treestore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include "utils/FileTree.hpp"

class GtkFileColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFileColumns()
	{
			add(m_col_size);
			add(m_col_percent);
			add(m_col_percent_text);
			add(m_col_priority);
			add(m_col_name);
			add(m_col_foreground);
			add(m_col_background);
			add(m_col_entry);
	}

	Gtk::TreeModelColumn<Glib::ustring> m_col_size;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<Glib::ustring> m_col_priority;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_background;
	Gtk::TreeModelColumn<Glib::ustring> m_col_foreground;
	Gtk::TreeModelColumn<libtorrent::file_entry> m_col_entry;
};

class GtkFileTreeView : public Gtk::TreeView
{
public:
	GtkFileColumns m_cols;
	Glib::RefPtr<Gtk::TreeStore> m_liststore;
	void select(std::shared_ptr<gt::Torrent> torrent);
	void loadColumns();
	void saveColumns();
	bool fileView_onClick(GdkEventButton *event);
	bool fileColumns_onClick(GdkEventButton *event);
	void setupColumns();
	void populateTree(FileTree &ft, Gtk::TreeRow *row);
	GtkFileTreeView();
};
