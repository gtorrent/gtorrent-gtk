#include "GtkTorrentInfoBar.hpp"
#include <gtkmm/label.h>

GtkTorrentInfoBar::GtkTorrentInfoBar()
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
    //TODO: better layout
    m_title = Gtk::manage(new Gtk::Label());
    this->pack_start(*m_title);

    m_progress = Gtk::manage(new GtkBlockBar());
    this->pack_start(*m_progress);
}

void GtkTorrentInfoBar::updateInfo(shared_ptr<gt::Torrent> t)
{
    if(t != nullptr)
    {
        this->set_visible(true);
        m_progress->setBlocks(t->getPieces());
        m_title->set_text(t->getHandle().name());
    }
    else
        this->set_visible(false);
}
