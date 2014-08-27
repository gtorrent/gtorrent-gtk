#include <gtorrent/Torrent.hpp>

#include <gtkmm/liststore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <glibmm.h>

class GtkTrackerColumns : public Gtk::TreeModel::ColumnRecord
{
  public:
    GtkTrackerColumns()
    {
      add(m_col_name);
      add(m_col_status);
      add(m_col_update_in);
      add(m_col_seeds);
      add(m_col_peers);
      add(m_col_interval);
      add(m_col_min_interval);
      add(m_col_downloaded);
    }

    Gtk::TreeModelColumn<std::string> m_col_name;
    Gtk::TreeModelColumn<std::string> m_col_status;
    Gtk::TreeModelColumn<std::string> m_col_update_in;
    Gtk::TreeModelColumn<std::string> m_col_seeds;
    Gtk::TreeModelColumn<std::string> m_col_peers;
    Gtk::TreeModelColumn<std::string> m_col_interval;
    Gtk::TreeModelColumn<std::string> m_col_min_interval;
    Gtk::TreeModelColumn<std::string> m_col_downloaded;

};

class GtkTrackerTreeView : public Gtk::TreeView
{
  private:
    GtkTrackerColumns m_cols;
    Glib::RefPtr<Gtk::ListStore> m_liststore;
    void setupColumns();
    //void insert(const gt::Tracker &p);
    std::shared_ptr<gt::Torrent> torrent;
  public:
    void update();
    void select(std::shared_ptr<gt::Torrent> t);
    GtkTrackerTreeView();
};
