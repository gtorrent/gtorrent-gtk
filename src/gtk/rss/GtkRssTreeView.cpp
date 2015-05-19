#include <gtkmm/liststore.h>
#include "GtkRssTreeView.hpp"

GtkRssTreeView::GtkRssTreeView(GtkTreeView *treeview, const Glib::RefPtr<Gtk::Builder> rbuilder)
 : Gtk::TreeView(treeview)
{
	auto list_store = Gtk::ListStore::create(m_cols);

	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(rbuilder->get_object("rss_col_name"))    ->pack_start(m_cols.m_col_name);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(rbuilder->get_object("rss_col_url"))     ->pack_start(m_cols.m_col_url);

	for (auto &c : this->get_columns()) {
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		c->set_alignment(0.5f);
		c->set_clickable();
		c->set_resizable();
		c->set_reorderable();

		Gtk::CellRendererText *r = dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell());
	}

	set_model(list_store);

	show_all_children();
}

