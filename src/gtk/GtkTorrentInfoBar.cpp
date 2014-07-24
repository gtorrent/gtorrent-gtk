#include "GtkTorrentInfoBar.hpp"

GtkTorrentInfoBar::GtkTorrentInfoBar()
{
    m_progress = Gtk::manage(new GtkBlockBar());
    this->pack_start(*m_progress);
}

void GtkTorrentInfoBar::updateInfo(shared_ptr<gt::Torrent> t)
{
    if(t != nullptr)
        m_progress->setBlocks(t->getPieces());
    else
        m_progress->setBlocks({false});
}
