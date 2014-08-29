#include "GtkPeerTreeView.hpp"

#include <gtkmm/liststore.h>

// Sort will be a bit fucked because the Peer class was written without being able to keep numerical values of the speed
// nor the ip, so for example 2 B/S is considered faster than 100 kB/s and 123.42.21.54 is considered lower than 82.12.54.31
void GtkPeerTreeView::insert(const gt::Peer &p)
{
	Gtk::TreeModel::iterator i = m_liststore->children().begin();
	while(i != m_liststore->children().end() && std::string((*i)[m_cols.m_col_ip]) < p.ip)
		++i;
	if(i != m_liststore->children().end() && std::string((*i)[m_cols.m_col_ip]) == p.ip)
	{
		(*i)[m_cols.m_col_ip] = p.ip;
		(*i)[m_cols.m_col_client] = p.client;
		(*i)[m_cols.m_col_down] = p.downloadSpeed;
		(*i)[m_cols.m_col_up] = p.uploadSpeed;
	}
	else
	{
		i = m_liststore->insert(i);
		(*i)[m_cols.m_col_ip] = p.ip;
		(*i)[m_cols.m_col_client] = p.client;
		(*i)[m_cols.m_col_down] = p.downloadSpeed;
		(*i)[m_cols.m_col_up] = p.uploadSpeed;
	}
}

GtkPeerTreeView::GtkPeerTreeView(GtkTreeView *tree, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::TreeView(tree) 
{
	m_liststore = Gtk::ListStore::create(m_cols);
	set_model(m_liststore);
	setupColumns();
}


void GtkPeerTreeView::update()
{
	m_liststore->clear();

	for (auto &peer : torrent->getPeers())
	{
		Gtk::TreeModel::iterator i = m_liststore->append();
		Gtk::TreeModel::Row row = *i;
		std::cout << peer.ip << std::endl
			<< peer.client << std::endl
			<< peer.downloadSpeed << std::endl;
		row[m_cols.m_col_ip] = peer.ip;
		row[m_cols.m_col_client] = peer.client;
		row[m_cols.m_col_down] = peer.downloadSpeed;
		row[m_cols.m_col_up] = peer.uploadSpeed;

	}

	for (auto &peer : torrent->getPeers())
		insert(peer);
}

void GtkPeerTreeView::setupColumns()
{
	int cid;
	Gtk::TreeViewColumn *col;
	cid = append_column("IP"      , m_cols.m_col_ip);
	col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_ip);
	append_column("Port", m_cols.m_col_port);
	cid = append_column("Client"  , m_cols.m_col_client);
	col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_client);
	append_column("Flags", m_cols.m_col_flags);
	append_column("%", m_cols.m_col_percent);
	append_column("Relevance", m_cols.m_col_relevance);
	cid = append_column("Down Speed", m_cols.m_col_down);
	col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_down);
	cid = append_column("Up Speed"  , m_cols.m_col_up);
	col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_up);
	append_column("Reqs", m_cols.m_col_reqs);
	append_column("Waited", m_cols.m_col_waited);
	append_column("Downloaded", m_cols.m_col_downloaded);
	append_column("Uploaded", m_cols.m_col_uploaded);
	append_column("Hasherr", m_cols.m_col_hasherr);
	append_column("Peer Download Rate", m_cols.m_col_peer_download_rate);
	append_column("MaxDown", m_cols.m_col_max_down);
	append_column("MaxUp", m_cols.m_col_max_up);
	append_column("Queued", m_cols.m_col_queued);
	append_column("Inactive", m_cols.m_col_inactive);
	append_column("Debug", m_cols.m_col_debug);	
}

void GtkPeerTreeView::select(std::shared_ptr<gt::Torrent> t)
{
	torrent = t;
	m_liststore->clear();
	update();
}
