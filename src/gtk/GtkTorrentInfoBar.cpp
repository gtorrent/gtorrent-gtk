#include "GtkTorrentInfoBar.hpp"
#include <gtkmm/label.h>
#include <Application.hpp>

GtkTorrentInfoBar::GtkTorrentInfoBar()
	: Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	//TODO: better layout
	m_title = Gtk::manage(new Gtk::Label());
	this->pack_start(*m_title);

	m_progress = Gtk::manage(new GtkBlockBar());
	this->pack_start(*m_progress);

	m_graph = Gtk::manage(new GtkGraph());
	this->pack_end(*m_graph, Gtk::PACK_EXPAND_WIDGET, 5);
}

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
		m_progress->setBlocks(t[selectedIndex]->getPieces());
		m_title->set_text(t[selectedIndex]->getHandle().name());
		m_graph->select(selectedIndex);
	}
	else
		this->set_visible(false);
}
