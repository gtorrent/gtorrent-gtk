#include <gtkmm/stylecontext.h>
#include <gtkmm/cssprovider.h>

#include "GtkTorrentSideBar.hpp"


GtkTorrentSideBar::GtkTorrentSideBar(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::TreeView(tree)
{
	rbuilder->get_widget_derived("GtkMainWindow", m_parent);
	m_liststore = Gtk::ListStore::create(cols);
	set_headers_visible(true);
	setupColumns();
	set_model(m_liststore);
	set_activate_on_single_click();
	override_color(get_style_context()->get_background_color());
	override_background_color(get_style_context()->get_color());
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
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererText *cell = Gtk::manage(new Gtk::CellRendererText());

	cell->set_padding(10, 0);
	cid = append_column(*Gtk::manage(new Gtk::TreeViewColumn("")));
	col = get_column(cid - 1);
	col->pack_start(*cell);
	col->add_attribute(cell->property_markup(), cols.name);

	Gtk::TreeModel::Row row = *(m_liststore->append());
	row[cols.name] = "<big>Torrents</big>";
	row[cols.title] = true;
	row[cols.clickCallback] = [](){std::cout << "sFSDF" << std::endl;}; // clicks on titles don't do shit

	row = *(m_liststore->append());
	row[cols.name] = "<big>RSS Feeds</big>";
	row[cols.title] = true;
	row[cols.clickCallback] = [](){}; // clicks on titles don't do shit

	//Maybe migrate settings there
/*	row = *(m_liststore->append());
	row[cols.name] = "<big>Settings</big>";
	row[cols.title] = true;
	row[cols.clickCallback] = [](){}; // clicks on titles don't do shit*/
}
