#include <gtkmm/icontheme.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/cssprovider.h>

#include "GtkTorrentSideBar.hpp"

GtkTorrentSideBar::GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::TreeView(tree), builder(rbuilder)
{
	rbuilder->get_widget_derived("GtkMainWindow", m_parent);
	rbuilder->get_widget_derived("rssDialog", m_rss);
	m_rss->set_transient_for(*m_parent);

	m_liststore = Gtk::TreeStore::create(cols);
	set_headers_visible(false);
	setupColumns();
	set_model(m_liststore);
	set_activate_on_single_click();
	set_show_expanders(false);
	expand_all();
	signal_row_activated().connect([this](const Gtk::TreeModel::Path& Path, Gtk::TreeViewColumn *col){ this->onRowClicked(*m_liststore->get_iter(Path)); });
}

void GtkTorrentSideBar::onRowClicked(Gtk::TreeRow clickedRow)
{
	std::function<void(void)> callback = clickedRow[cols.clickCallback];
	callback();
}

void GtkTorrentSideBar::setupColumns()
{
	int cid = 0;
	Gtk::TreeViewColumn     *col = nullptr;
	Gtk::CellRendererText   *cell = Gtk::manage(new Gtk::CellRendererText());
	Gtk::CellRendererPixbuf *cellp = Gtk::manage(new Gtk::CellRendererPixbuf());

	cell->set_alignment(0, 0.5);
	cell->signal_edited().connect([this](const std::string& path,const std::string& name){ addedItem(path, name); });

	cid = append_column(*Gtk::manage(new Gtk::TreeViewColumn("Name")));	
	col = get_column(cid - 1);
	col->pack_start(*cellp);
	col->pack_start(*cell);
	col->add_attribute(cell->property_markup(), cols.name);
	col->add_attribute(cell->property_editable(), cols.editable);
	col->add_attribute(cellp->property_pixbuf(), cols.icon);

	Torrents = *(m_liststore->append());
	Torrents[cols.name] = "<big>Torrents</big>";
	Torrents[cols.editable] = false;
	Torrents[cols.clickCallback] = [](){}; // clicks on titles don't do shit

	Gtk::TreeModel::Row row = *(m_liststore->append(Torrents.children()));
	row[cols.name] = "Add a label";
	row[cols.editable] = true;
	row[cols.icon] = Gtk::IconTheme::get_default()->lookup_icon("list-add-symbolic", 16, Gtk::ICON_LOOKUP_USE_BUILTIN).load_icon();
	row[cols.clickCallback] = [row](){};

	RSSFeeds = *(m_liststore->append());
	RSSFeeds[cols.name] = "<big>RSS Feeds</big>";
	RSSFeeds[cols.editable] = false;
	RSSFeeds[cols.clickCallback] = [](){};

	row = *(m_liststore->append(RSSFeeds.children()));
	row[cols.name] = "Add an RSS group";
	row[cols.editable] = true;
	row[cols.icon] = Gtk::IconTheme::get_default()->lookup_icon("list-add-symbolic", 16, Gtk::ICON_LOOKUP_USE_BUILTIN).load_icon();
	row[cols.clickCallback] = [row](){};

	//Maybe migrate settings there
/*	row = *(m_liststore->append());
	row[cols.name] = "<big>Settings</big>";
	row[cols.title] = true;
	row[cols.clickCallback] = [](){}; // clicks on titles don't do shit*/
}

void GtkTorrentSideBar::addedItem(std::string path, std::string name)
{
	// Depending on the parent, we create a label or an rss group
	// TreeRows are iterators, so comparison is valid.
	Gtk::TreeModel::Row row = *m_liststore->get_iter(path);
	if(row->parent() && *row->parent() == Torrents)
	{
		Gtk::TreeModel::Row newrow = *(m_liststore->insert(row));
		newrow[cols.name] = name + " (" + '0' + ")";
		newrow[cols.editable] = true;
		newrow[cols.icon] = Glib::RefPtr<Gdk::Pixbuf>();
		newrow[cols.clickCallback] = [row](){};
		// TODO: Pop up a new label dialog.
		// Torrent should be added a label by drag and dropping them into the sidebar
		// see enable_model_drag_source
		// and enable_model_drag_dest
	}
	else if(row->parent() && *row->parent() == RSSFeeds)
	{
		Gtk::TreeModel::Row newrow = *(m_liststore->insert(row));
		newrow[cols.name] = name + " (" + '0' + ")";
		newrow[cols.editable] = true;
		newrow[cols.icon] = Glib::RefPtr<Gdk::Pixbuf>();
		newrow[cols.clickCallback] = [row](){};
		m_rss->set_default_response(1);
		m_rss->run(name);
		abort();
		// TODO: implement RSS dialog and pop it up here
	}
}
