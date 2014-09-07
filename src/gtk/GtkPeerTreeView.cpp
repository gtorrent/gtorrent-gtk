#include "GtkPeerTreeView.hpp"

#include <gtkmm/liststore.h>

void GtkPeerTreeView::insert(const gt::Peer &p)
{
	Gtk::TreeModel::iterator i = m_liststore->children().begin();
	while(i != m_liststore->children().end() && boost::asio::ip::address::from_string(std::string((*i)[m_cols.m_col_ip])) < p.getIP())
		++i;
	if(i != m_liststore->children().end() && boost::asio::ip::address::from_string(std::string((*i)[m_cols.m_col_ip])) == p.getIP())
	{
		(*i)[m_cols.m_col_ip] = p.getIP().to_string();
		(*i)[m_cols.m_col_client] = p.getClient();
		(*i)[m_cols.m_col_down] = getRateString(p.getDownloadSpeed());
		(*i)[m_cols.m_col_up] = getRateString(p.getUploadSpeed());
	}
	else
	{
		i = m_liststore->insert(i);
		(*i)[m_cols.m_col_ip] = p.getIP().to_string();
		(*i)[m_cols.m_col_client] = p.getClient();
		(*i)[m_cols.m_col_down] = getRateString(p.getDownloadSpeed());
		(*i)[m_cols.m_col_up] = getRateString(p.getUploadSpeed());
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
	queue_draw();
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
