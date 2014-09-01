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
#include "GtkGeneralBox.hpp"
#include "GtkPeerTreeView.hpp"
#include "GtkFileTreeView.hpp"

/**
* Sets up the torrent info bar.
*/
GtkTorrentInfoBar::GtkTorrentInfoBar(GtkBox *box, const Glib::RefPtr<Gtk::Builder> rbuilder)
	: Gtk::Box(box), builder(rbuilder)
{
	builder->get_widget        ("infoBarTitle"       , m_title     );
	builder->get_widget_derived("infoBarGraph"       , m_graph     );
	builder->get_widget_derived("infoBarBlockBar"    , m_progress  );
	builder->get_widget_derived("infoBarPeerTreeView", m_peers     );
	builder->get_widget_derived("infoBarFileTreeView", m_fileview  );
	builder->get_widget_derived("infoBarStatusBox"   , m_general_box);
	show_all();
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
		set_visible();
	else
	{
		set_visible();
		return;
	}

	if(selected->status().has_metadata) // torrentless torrents (magnet links) can't have pieces
		m_progress->setBlocks(selected->getPieces());

	m_title->set_text(selected->status().name);
	m_graph->select(selected);
	m_peers->select(selected);
	m_fileview->select(selected);
	if(previous != selected)
		m_general_box->update(selected);
	previous = selected;
	show_all();
}

void GtkTorrentInfoBar::updateState(std::shared_ptr<gt::Torrent> selected)
{
	if(!selected) return updateInfo(selected);
	if(selected->status().has_metadata)
		m_progress->setBlocks(selected->getPieces());

	m_general_box->update(selected);
	m_peers->update();
	m_fileview->update();
	std::vector<std::shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for(auto ptr : t)
		m_graph->addValue(ptr, (double)ptr->getUploadRate(), (double)ptr->getDownloadRate());

}
