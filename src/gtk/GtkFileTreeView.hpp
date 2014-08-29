#include <gtorrent/Torrent.hpp>

#include <gtkmm/treestore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include "utils/FileTree.hpp"
#include <glibmm.h>

class GtkFileColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFileColumns()
	{
		//name,path,size,done,%,first piece,# pieces,pieces, priority, mode
			add(m_col_name);
			add(m_col_fullpath);
			add(m_col_size);
			add(m_col_percent);
			add(m_col_percent_text);
			add(m_col_priority);
			add(m_col_prioritylevel);
			add(m_col_entry);
			add(m_col_activated);
			add(m_col_inconsistent);
			add(m_col_folder);
			add(m_col_icon);
			add(m_col_index);
	}

	/* Shown column */
	Gtk::TreeModelColumn<Glib::ustring> m_col_size;

	/* Columns that use a custom cell renderer */
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<Glib::ustring> m_col_priority;

	/* Colomns used to keep data */
	Gtk::TreeModelColumn<libtorrent::file_entry> m_col_entry;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int> m_col_prioritylevel;
	Gtk::TreeModelColumn<unsigned int> m_col_percent;
	Gtk::TreeModelColumn<unsigned int> m_col_index;
	Gtk::TreeModelColumn<std::string> m_col_fullpath;
	Gtk::TreeModelColumn<bool> m_col_activated;
	Gtk::TreeModelColumn<bool> m_col_inconsistent;
	Gtk::TreeModelColumn<bool> m_col_folder;
	Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_col_icon;

};

class GtkFileTreeView : public Gtk::TreeView
{
public:
	std::shared_ptr<gt::Torrent> torrent;
	GtkFileColumns m_cols;
	Glib::RefPtr<Gtk::TreeStore> m_liststore;
	std::vector<libtorrent::size_type> progress_all;
	Gtk::Menu *m_rcMenu;

	void select(std::shared_ptr<gt::Torrent> selected);
	void loadColumns();
	void saveColumns();
	bool fileView_onClick(GdkEventButton *event);
	bool fileColumns_onClick(GdkEventButton *event);
	void setupColumns();
	void setPriority(Gtk::TreeRow &node, int level);
	void update();
	void update(Gtk::TreeRow &row);
	void onCheckBoxClicked(std::string path);
	void populateTree(FileTree &ft, Gtk::TreeRow *row);
	void getChildAttributes(FileTree &ft, long &size, int &state, double &progress, int &priority);
	void getChildAttributes(Gtk::TreeRow &row, long &size, int &state, double &progress, int &priority);
	void toggleView_onClick();
	void openView_onClick();
	std::vector<Gtk::TreeRow> selectedRows();
	void  setSelectedPriorities(int);
	GtkFileTreeView();
};
