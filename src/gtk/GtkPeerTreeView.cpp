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

GtkPeerTreeView::GtkPeerTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	set_model(m_liststore);
	setupColumns();
}


void GtkPeerTreeView::update()
{
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
	cid = append_column("Client"  , m_cols.m_col_client);
    col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_client);
	cid = append_column("Download", m_cols.m_col_down);
    col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_down);
	cid = append_column("Upload"  , m_cols.m_col_up);
    col = get_column(cid - 1);
	col->set_sort_column(m_cols.m_col_up);
}

void GtkPeerTreeView::select(std::shared_ptr<gt::Torrent> t)
{
	torrent = t;
	m_liststore->clear();
	update();
}
