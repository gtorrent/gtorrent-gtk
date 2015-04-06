#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>

#include "GtkMainWindow.hpp"
#include "rss/GtkRSSDialog.hpp"
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
                        add(title);
                        add(group);
                        add(group_vector);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;

	Gtk::TreeModelColumn<Glib::ustring> filter; // used to filter the main treeview
	Gtk::TreeModelColumn<bool> editable; // Uneditable items are only titles and already existing labels/groups // TODO Nyanpasu: I'm probably going to remove this in favour of a context menu or some sort of group management dialog
	Gtk::TreeModelColumn<std::function<void(void)>> clickCallback;
	Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> icon;

        // And here is where liststore starts to fucking suck.
        // Only a particular row of each member needs this.
        // Furthermore, these are unset for irrelevant rows. Not sure what they
        // are supposed to default to.
        Gtk::TreeModelColumn<gt::TorrentGroup*> group;
	Gtk::TreeModelColumn<Glib::ustring> title;
        Gtk::TreeModelColumn<std::vector<std::shared_ptr<gt::Torrent>>*> group_vector; // Nice templates.
};

class GtkTorrentSideBar : public Gtk::TreeView
{
	GtkRSSDialog  *m_rss    = nullptr;
	GtkMainWindow *m_parent = nullptr;
	Glib::RefPtr<Gtk::Builder> m_builder;
public:
	GtkSideBarColumns m_cols;
	Glib::RefPtr<Gtk::TreeStore> m_liststore;

	Gtk::TreeModel::Row m_torrent_row;
	Gtk::TreeModel::Row m_rssfeed_row;

	GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder);
        bool sideBar_onClick(GdkEventButton *event);
        void addedItem(std::string path, std::string name);
        void onRowClicked(Gtk::TreeRow);
        void setupColumns();
        void updateTorrents();
};
