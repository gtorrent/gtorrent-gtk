#pragma once

#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>

class GtkRssColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkRssColumns()
	{
		add(m_col_queue);
		add(m_col_name);
		add(m_col_url);
	}

	Gtk::TreeModelColumn<unsigned int>  m_col_queue;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<Glib::ustring> m_col_url;
};

class GtkRssTreeView : public Gtk::TreeView
{
public:
	GtkRssColumns m_cols;
	GtkRssTreeView(GtkTreeView *treeview, const Glib::RefPtr<Gtk::Builder> rbuilder);
};
