#include <gtkmm/liststore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>

#include "GtkMainWindow.hpp"
#include "GtkTorrentTreeView.hpp"

class GtkSideBarColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkSideBarColumns()
	{
			add(name);
			add(title);
			add(clickCallback);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<bool> title;
	Gtk::TreeModelColumn<std::function<void(void)>> clickCallback;
};

class GtkTorrentSideBar : public Gtk::TreeView
{
	GtkMainWindow *m_parent = nullptr;
public:
	GtkSideBarColumns cols;
	Glib::RefPtr<Gtk::ListStore> m_liststore;
	bool sideBar_onClick(GdkEventButton *event);
	GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder);
	void setupColumns();
	void onRowClicked(Gtk::TreeRow);
};
