#include <gtorrent/Torrent.hpp>

#include <gtkmm/builder.h>
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
			add(m_col_port);
			add(m_col_client);
			add(m_col_flags);
			add(m_col_percent);
			add(m_col_relevance);
			add(m_col_down);
			add(m_col_up);
			add(m_col_reqs);
			add(m_col_waited);
			add(m_col_downloaded);
			add(m_col_uploaded);
			add(m_col_hasherr);
			add(m_col_peer_download_rate);
			add(m_col_max_down);
			add(m_col_max_up);
			add(m_col_queued);
			add(m_col_inactive);
			add(m_col_debug);
		}

		Gtk::TreeModelColumn<std::string> m_col_ip;
		Gtk::TreeModelColumn<std::string> m_col_port;
		Gtk::TreeModelColumn<std::string> m_col_client;
		Gtk::TreeModelColumn<std::string> m_col_flags;
		Gtk::TreeModelColumn<std::string> m_col_percent;
		Gtk::TreeModelColumn<std::string> m_col_relevance;
		Gtk::TreeModelColumn<std::string> m_col_down;
		Gtk::TreeModelColumn<std::string> m_col_up;
		Gtk::TreeModelColumn<std::string> m_col_reqs;
		Gtk::TreeModelColumn<std::string> m_col_waited;
		Gtk::TreeModelColumn<std::string> m_col_downloaded;
		Gtk::TreeModelColumn<std::string> m_col_uploaded;
		Gtk::TreeModelColumn<std::string> m_col_hasherr;
		Gtk::TreeModelColumn<std::string> m_col_peer_download_rate;
		Gtk::TreeModelColumn<std::string> m_col_max_down;
		Gtk::TreeModelColumn<std::string> m_col_max_up;
		Gtk::TreeModelColumn<std::string> m_col_queued;
		Gtk::TreeModelColumn<std::string> m_col_inactive;
		Gtk::TreeModelColumn<std::string> m_col_debug;

};

class GtkPeerTreeView : public Gtk::TreeView
{
	private:
		GtkPeerColumns m_cols;
		Glib::RefPtr<Gtk::ListStore> m_liststore;
		void setupColumns();
		void insert(const gt::Peer &p);
		std::shared_ptr<gt::Torrent> torrent;
	public:
		void update();
		void select(std::shared_ptr<gt::Torrent> t);
		GtkPeerTreeView(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder);
};
