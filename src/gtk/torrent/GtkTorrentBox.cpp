#include "GtkTorrentBox.hpp"
#include "../../Application.hpp"

#include <gtkmm/builder.h>
#include <libnotify/notification.h>

GtkTorrentBox::GtkTorrentBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder) :
	Gtk::Box(box),
	builder(refBuilder),
	m_core(Application::getSingleton()->getCore())
{
	show_all_children(true);

	builder->get_widget_derived("torrent_infobar", m_infobar);
	builder->get_widget_derived("treeview_torrent", m_treeview_torrent);

	builder->get_widget("panel", m_panel);
	builder->get_widget("searchbar", m_searchbar);
	builder->get_widget("torrentbox", m_torrentbox);

	m_panel->add(*m_infobar);
}

void GtkTorrentBox::searchToggle()
{
	m_searchbar->set_search_mode(!m_searchbar->get_search_mode());
}

void GtkTorrentBox::updateTorrents()
{
	// Update the gooey
	m_treeview_torrent->updateCells();
	m_infobar->updateState(m_treeview_torrent->getFirstSelected());

	// TODO Handle all torrents from m_core->update
	std::shared_ptr<gt::Torrent> t = m_core->update();
	if (t) torrentAdd(t);
}

void GtkTorrentBox::torrentAdd(std::shared_ptr<gt::Torrent> t)
{
	t->onStateChanged = [this](int oldstate, std::shared_ptr<gt::Torrent> t){ onTorrentStateChange(oldstate, t); };
	m_treeview_torrent->addCell(t);
}

void GtkTorrentBox::onClickPause()
{
	m_treeview_torrent->setSelectedPaused(true);
	m_treeview_torrent->onSelectionChanged();
}

void GtkTorrentBox::onClickResume()
{
	m_treeview_torrent->setSelectedPaused(false);
	m_treeview_torrent->onSelectionChanged();
}

void GtkTorrentBox::onClickRemove()
{
	for(auto t : m_treeview_torrent->selectedTorrents())
		m_infobar->removeInfo(t);
	m_treeview_torrent->removeSelected();
}

void GtkTorrentBox::saveColumns()
{
       m_treeview_torrent->saveColumns();
}

void GtkTorrentBox::onTorrentStateChange(int oldstate, std::shared_ptr<gt::Torrent> t)
{
	NotifyNotification *tNotify = nullptr;

	int newstate = t->status().state;

	if(newstate == libtorrent::torrent_status::seeding && oldstate == libtorrent::torrent_status::downloading)
		tNotify = notify_notification_new (t->status().name.c_str(), std::string(t->status().name + " has finished downloading.").c_str(), "dialog-information");
	else if(newstate == libtorrent::torrent_status::downloading  &&
		oldstate == libtorrent::torrent_status::downloading_metadata)
		tNotify = notify_notification_new (t->status().name.c_str(), std::string(t->status().name + " has started downloading.").c_str(), "dialog-information");
	else
		return; //:^)

	notify_notification_show (tNotify, NULL);
	g_object_unref(G_OBJECT(tNotify));
}
