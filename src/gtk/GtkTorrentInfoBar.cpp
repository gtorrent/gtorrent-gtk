#include "GtkTorrentInfoBar.hpp"

#include <gtkmm/box.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/table.h>
#include <gtkmm/scrolledwindow.h>

#include "../Application.hpp"
#include "GtkBlockBar.hpp"
#include "GtkGraph.hpp"
#include "GtkStatusBox.hpp"

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
	m_progress->set_size_request(0, 20);


	m_stat_box->add(*m_piece_box);

	m_scroll_box = Gtk::manage(new Gtk::ScrolledWindow());
	m_status_box = Gtk::manage(new GtkStatusBox());
	m_scroll_box->add(*m_status_box);

	m_stat_box->pack_start(*(new Gtk::HSeparator()), Gtk::PACK_SHRINK);
	m_stat_box->pack_start(*m_scroll_box, Gtk::PACK_EXPAND_WIDGET);
	m_notebook->append_page(*m_graph, "Info Graph");
	m_notebook->append_page(*m_stat_box, "Torrent Info");
	this->pack_end(*m_notebook, Gtk::PACK_EXPAND_WIDGET, 5);
}

// TODO: Should replace every place where a torrent index is required with a torrent pointer, smells like everything would break if
// the user tried to sort the torrents.

/**
* Updates the torrent info bar.
*/
void GtkTorrentInfoBar::updateInfo(std::shared_ptr<gt::Torrent> selected)
{
	static std::shared_ptr<gt::Torrent> previous = nullptr;

	if(selected)
		set_visible(true);
	else
	{
		set_visible(false);
		return;
	}

	if(selected->getHandle().status().has_metadata) // torrentless torrents (magnet links) can't have pieces
		m_progress->setBlocks(selected->getPieces());

	m_title->set_text(selected->getName());
	m_graph->select(selected);

	if(previous != selected)
		m_status_box->update(selected);
	previous = selected;
}

void GtkTorrentInfoBar::updateState(std::shared_ptr<gt::Torrent> selected)
{
	if(!selected) return updateInfo(selected);
	if(selected->getHandle().status().has_metadata)
		m_progress->setBlocks(selected->getPieces());

	m_status_box->update(selected);

	std::vector<std::shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for(auto ptr : t)
		m_graph->add(ptr, (double)ptr->getUploadRate(), (double)ptr->getDownloadRate());

}
