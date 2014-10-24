#include <gtkmm/icontheme.h>
#include <gtkmm/stylecontext.h>
#include <gtkmm/cssprovider.h>

#include "GtkTorrentSideBar.hpp"
#include "../Application.hpp"

GtkTorrentSideBar::GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder)
  : Gtk::TreeView(tree), m_builder(rbuilder)
{
	rbuilder->get_widget_derived("GtkMainWindow", m_parent);
	rbuilder->get_widget_derived("rssDialog", m_rss);
	m_rss->set_transient_for(*m_parent);

	m_liststore = Gtk::TreeStore::create(m_cols);

	set_model(m_liststore);

	set_headers_visible(false);
	set_show_expanders(true);
	set_activate_on_single_click();
	expand_all(); // Shouldn't this go after setupColumns()?

	signal_row_activated().connect([this](const Gtk::TreeModel::Path& Path, Gtk::TreeViewColumn *col){ this->onRowClicked(*m_liststore->get_iter(Path)); });

	setupColumns();
}

void GtkTorrentSideBar::onRowClicked(Gtk::TreeRow clickedRow)
{
	std::function<void(void)> callback = clickedRow[m_cols.clickCallback];
	callback();
}

// Cringe.
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
	col->add_attribute(cell->property_markup(), m_cols.name);
	col->add_attribute(cell->property_editable(), m_cols.editable);
	col->add_attribute(cellp->property_pixbuf(), m_cols.icon);

        // This really isn't "setupColumns" anymore from this point.
        // TODO Move to own function
	m_torrent_row = *(m_liststore->append());
	m_torrent_row[m_cols.name] = "Torrents";
	m_torrent_row[m_cols.editable] = false;
	m_torrent_row[m_cols.clickCallback] = [](){}; // clicks on titles don't do shit

	auto torrent_icon = Gdk::Pixbuf::create_from_resource("/org/gtk/gtorrent/icon-torrent.png");
	auto torrent_icon_scaled =  torrent_icon->scale_simple(16, 16, Gdk::INTERP_BILINEAR);
	m_torrent_row[m_cols.icon] = torrent_icon_scaled;

        // XXX TMP WILL REMOVE AND REPLACE WITH PROPER FUNCTION
        // Yes this is horrible. Bear with it for now.
        auto g = Application::getSingleton()->getCore()->getAllTorrents();
	Gtk::TreeModel::Row row = *(m_liststore->append(m_torrent_row.children()));
	row[m_cols.name] = "All";
	row[m_cols.title] = "All";
	row[m_cols.group] = g;
	row[m_cols.group_vector] = &g->m_torrents_all;

	Gtk::TreeModel::Row row2 = *(m_liststore->append(row.children()));
	row2[m_cols.name] = "Downloading";
	row2[m_cols.title] = "Downloading";
	row2[m_cols.group_vector] = &g->m_torrents_downloading;
        row2 = *(m_liststore->append(row.children()));
        row2[m_cols.name] = "Seeding";
        row2[m_cols.title] = "Seeding";
        row2[m_cols.group_vector] = &g->m_torrents_seeding;
        row2 = *(m_liststore->append(row.children()));
        row2[m_cols.name] = "Checking";
        row2[m_cols.title] = "Checking";
        row2[m_cols.group_vector] = &g->m_torrents_checking;
        row2 = *(m_liststore->append(row.children()));
        row2[m_cols.name] = "Finished";
        row2[m_cols.title] = "Finished";
        row2[m_cols.group_vector] = &g->m_torrents_finished;
        row2 = *(m_liststore->append(row.children()));
        row2[m_cols.name] = "Stopped";
        row2[m_cols.title] = "Stopped";
        row2[m_cols.group_vector] = &g->m_torrents_stopped;
        row2 = *(m_liststore->append(row.children()));
        row2[m_cols.name] = "Paused";
        row2[m_cols.title] = "Paused";
        row2[m_cols.group_vector] = &g->m_torrents_paused;

        // End of new horrible code
        // Continue horrible code from before
	//row = *(m_liststore->append(m_torrent_row.children()));
	//row[m_cols.name] = "Add a label";
	//row[m_cols.editable] = true;
	//row[m_cols.icon] = Gtk::IconTheme::get_default()->lookup_icon("list-add-symbolic", 16, Gtk::ICON_LOOKUP_USE_BUILTIN).load_icon();
	//row[m_cols.clickCallback] = [row](){};

	m_rssfeed_row = *(m_liststore->append());
	m_rssfeed_row[m_cols.name] = "RSS Feeds";
	m_rssfeed_row[m_cols.editable] = false;
	m_rssfeed_row[m_cols.clickCallback] = [](){};

	auto rss_icon = Gdk::Pixbuf::create_from_resource("/org/gtk/gtorrent/icon-rss.png");
	auto rss_icon_scaled =  rss_icon->scale_simple(16, 16, Gdk::INTERP_BILINEAR);
	m_rssfeed_row[m_cols.icon] = rss_icon_scaled;

	for(auto fg : Application::getSingleton()->getCore()->m_feeds)
	{
		row = *(m_liststore->append(m_rssfeed_row.children()));
		row[m_cols.name] = fg->name;
		row[m_cols.editable] = false;
		// TODO change icon to some sort of generic RSS icon
		row[m_cols.clickCallback] = [this, fg](){m_rss->run(fg->name);m_rss->hide();};

	}
	
	row = *(m_liststore->append(m_rssfeed_row.children()));
	row[m_cols.name] = "Add an RSS group";
	row[m_cols.editable] = true;

	row[m_cols.icon] = Gtk::IconTheme::get_default()->lookup_icon("list-add-symbolic", 16, Gtk::ICON_LOOKUP_USE_BUILTIN).load_icon();
	row[m_cols.clickCallback] = [row](){};

	//Maybe migrate settings there
/*	row = *(m_liststore->append());
	row[m_cols.name] = "Settings";
	row[m_cols.title] = true;
	row[m_cols.clickCallback] = [](){}; // clicks on titles don't do shit*/
}

void GtkTorrentSideBar::addedItem(std::string path, std::string name)
{
	// Depending on the parent, we create a label or an rss group
	// TreeRows are iterators, so comparison is valid.
	Gtk::TreeModel::Row row = *m_liststore->get_iter(path);
	if(row->parent() && *row->parent() == m_torrent_row)
	{
		Gtk::TreeModel::Row newrow = *(m_liststore->insert(row));
		newrow[m_cols.name] = name + " (" + '0' + ")";
		newrow[m_cols.editable] = true;
		newrow[m_cols.icon] = Glib::RefPtr<Gdk::Pixbuf>();
		newrow[m_cols.clickCallback] = [row](){};
		// TODO: Pop up a new label dialog.
		// Torrent should be added a label by drag and dropping them into the sidebar
		// see enable_model_drag_source
		// and enable_model_drag_dest
	}
	else if(row->parent() && *row->parent() == m_rssfeed_row)
	{
		Gtk::TreeModel::Row newrow = *(m_liststore->insert(row));
		newrow[m_cols.name] = name + " (" + '0' + ")";
		newrow[m_cols.editable] = true;
		newrow[m_cols.icon] = Glib::RefPtr<Gdk::Pixbuf>();
		newrow[m_cols.clickCallback] = [this, name](){m_rss->run(name);	m_rss->hide();};
		m_rss->set_default_response(1);
		int code = m_rss->run(name);
		m_rss->hide();
		if(code == 1)
			Application::getSingleton()->getCore()->m_feeds.push_back(m_rss->feedg);
		// TODO: implement RSS dialog and pop it up here
	}
}

void GtkTorrentSideBar::updateTorrents()
{
        for (auto r : m_torrent_row.children())
        {
                for(auto r2 : r.children())
                {
                        // TODO Create a proper function in TorrentGroup to get this.
                        std::stringstream o;
                        auto v = r2.get_value(m_cols.group_vector);
                        auto s =r2.get_value(m_cols.title);
                        o << s << " (" << v->size() << ")"; // Sorry. I would use sprintf, but sepples
                        r2[m_cols.name] = o.str();
                }
        }
}
