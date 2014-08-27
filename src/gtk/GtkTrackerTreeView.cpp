#include "GtkTrackerTreeView.hpp"

#include <gtkmm/liststore.h>
/*
void GtkTrackerTreeView::insert(const gt::Tracker &p)
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
}*/

GtkTrackerTreeView::GtkTrackerTreeView()
{
  m_liststore = Gtk::ListStore::create(m_cols);
  set_model(m_liststore);
  setupColumns();
}


void GtkTrackerTreeView::update()
{/*
  m_liststore->clear();

  for (auto &Tracker : torrent->getTrackers())
  {
    Gtk::TreeModel::iterator i = m_liststore->append();
    Gtk::TreeModel::Row row = *i;
    std::cout << Tracker.ip << std::endl
      << Tracker.client << std::endl
      << Tracker.downloadSpeed << std::endl;
    row[m_cols.m_col_ip] = Tracker.ip;
    row[m_cols.m_col_client] = Tracker.client;
    row[m_cols.m_col_down] = Tracker.downloadSpeed;
    row[m_cols.m_col_up] = Tracker.uploadSpeed;

  }

  for (auto &Tracker : torrent->getTrackers())
    insert(Tracker);*/
}

void GtkTrackerTreeView::setupColumns()
{
  append_column("Name", m_cols.m_col_name);
  append_column("Status", m_cols.m_col_status);
  append_column("Update In", m_cols.m_col_update_in);
  append_column("Seeds", m_cols.m_col_seeds);
  append_column("Peers", m_cols.m_col_peers);
  append_column("Interval", m_cols.m_col_interval);
  append_column("Min Interval", m_cols.m_col_min_interval);
  append_column("Downloaded", m_cols.m_col_downloaded);
}

void GtkTrackerTreeView::select(std::shared_ptr<gt::Torrent> t)
{
  torrent = t;
  m_liststore->clear();
  //update();
}
