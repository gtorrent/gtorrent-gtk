#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>

#include "GtkMainWindow.hpp"
#include "GtkRSSDialog.hpp"
#include "GtkTorrentTreeView.hpp"

class GtkSideBarColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkSideBarColumns()
	{
			add(name);
			add(editable);
			add(clickCallback);
			add(icon);
			add(filter);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;

	Gtk::TreeModelColumn<Glib::ustring> filter; // used to filter the main treeview
	Gtk::TreeModelColumn<bool> editable; // Uneditable items are only titles and already existing labels/groups // TODO Nyanpasu: I'm probably going to remove this in favour of a context menu or some sort of group management dialog
	Gtk::TreeModelColumn<std::function<void(void)>> clickCallback;
	Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> icon;
};

class GtkTorrentSideBar : public Gtk::TreeView
{
	GtkRSSDialog  *m_rss    = nullptr;
	GtkMainWindow *m_parent = nullptr;
	Glib::RefPtr<Gtk::Builder> builder;
public:
	GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder);
	void setupColumns();
	void onRowClicked(Gtk::TreeRow);
	void addedItem(std::string path, std::string name);
	bool sideBar_onClick(GdkEventButton *event);

	GtkSideBarColumns m_cols;
	Glib::RefPtr<Gtk::TreeStore> m_liststore;

	Gtk::TreeModel::Row m_torrent_row;
	Gtk::TreeModel::Row m_rssfeed_row;
};
