#include "GtkTorrentInfoBar.hpp"
#include <gtkmm/table.h>
#include <gtkmm/hvseparator.h>

/**
* Sets up the torrent info bar.
*/
GtkTorrentInfoBar::GtkTorrentInfoBar()
	: Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	//TODO: better layout
	m_notebook = Gtk::manage(new Gtk::Notebook());
	m_stat_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_piece_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	m_title = Gtk::manage(new Gtk::Label());

	this->pack_start(*m_title, Gtk::PACK_SHRINK);

	m_progress = Gtk::manage(new GtkBlockBar());
	m_graph = Gtk::manage(new GtkGraph());

    m_piece_box->pack_end(*m_progress, Gtk::PACK_EXPAND_WIDGET, 0);
    m_progress->set_size_request(0, 50);


    m_stat_box->add(*m_piece_box);

  	Gtk::Table *table_layout = Gtk::manage(new Gtk::Table(3, 2, false));
  	table_layout->set_col_spacings(5);

    Gtk::Label *down_total_label = Gtk::manage(new Gtk::Label());
    down_total_label->set_markup("<b>Downloaded</b>");
    Gtk::Label *down_total = Gtk::manage(new Gtk::Label("0"));
    table_layout->attach(*down_total_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
    table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
    table_layout->attach(*down_total, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);

    Gtk::Label *up_total_label = Gtk::manage(new Gtk::Label());
    up_total_label->set_markup("<b>Uploaded</b>");
    Gtk::Label *up_total = Gtk::manage(new Gtk::Label("0"));
    table_layout->attach(*up_total_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
    table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
    table_layout->attach(*up_total, 2, 3, 1, 2, Gtk::AttachOptions::SHRINK);

    m_stat_box->pack_start(*(new Gtk::HSeparator()), Gtk::PACK_SHRINK);
    m_stat_box->pack_start(*table_layout, Gtk::PACK_SHRINK);
	m_notebook->append_page(*m_graph, "Info Graph");
	m_notebook->append_page(*m_stat_box, "Torrent Info");
	this->pack_end(*m_notebook, Gtk::PACK_EXPAND_WIDGET, 5);
}

/**
* Updates the torrent info bar.
*/
void GtkTorrentInfoBar::updateInfo(shared_ptr<gt::Torrent> selected)
{
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();

	bool selectionExists = false;
	unsigned selectedIndex;
	for(unsigned i = 0; i < t.size(); ++i)
	{
		m_graph->add(i, (double)t[i]->getUploadRate(), (double)t[i]->getDownloadRate());
		if(selected == t[i])
		{
			selectedIndex = i;
			selectionExists = true;
		}
	}
	if(selectionExists)
	{
		this->set_visible(true);
		if(t[selectedIndex]->getHandle().status().has_metadata) // torrentless torrents (magnet links) can't have pieces
			m_progress->setBlocks(t[selectedIndex]->getPieces());
		m_title->set_text(t[selectedIndex]->getHandle().name());
		m_graph->select(selectedIndex);
	}
	else
		this->set_visible(false);
}
