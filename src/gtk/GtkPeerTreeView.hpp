#include <gtorrent/Torrent.hpp>

#include <gtkmm/liststore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <glibmm.h>

class GtkPeerColumns : public Gtk::TreeModel::ColumnRecord
{
	public:
		GtkPeerColumns()
		{
			add(m_col_ip);
			add(m_col_client);
			add(m_col_down);
			add(m_col_up);
		}

		Gtk::TreeModelColumn<std::string> m_col_ip;
		Gtk::TreeModelColumn<std::string> m_col_client;
		Gtk::TreeModelColumn<std::string> m_col_down;
		Gtk::TreeModelColumn<std::string> m_col_up;
};

class GtkPeerTreeView : public Gtk::TreeView
{
	private:
		GtkPeerColumns m_cols;
		Glib::RefPtr<Gtk::ListStore> m_liststore;
		void setupColumns();

	public:
		std::shared_ptr<gt::Torrent> torrent;
		void update();
		inline void select(std::shared_ptr<gt::Torrent> t)
		{
			torrent = t;
		}
		GtkPeerTreeView();
};
