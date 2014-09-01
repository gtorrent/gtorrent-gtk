#include "GtkTorrentTreeView.hpp"
#include <glibmm/pattern.h>
#include <gtorrent/Log.hpp>
#include <gtorrent/Settings.hpp>
#include <gtorrent/Platform.hpp>
#include <gtkmm/treerowreference.h>
#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/separatormenuitem.h>
#include <cctype>
#include "../Application.hpp"
#include "GtkMainWindow.hpp"
#include "GtkTorrentInfoBar.hpp"

/**
* Sets up the tree view containing torrent information.
*/
GtkTorrentTreeView::GtkTorrentTreeView(GtkTreeView *treeview, const Glib::RefPtr<Gtk::Builder> rbuilder) :
	Gtk::TreeView(treeview)
{
	m_liststore = Gtk::ListStore::create(m_cols);
	m_filter = Gtk::TreeModelFilter::create(m_liststore);
	m_filtersort = Gtk::TreeModelSort::create(m_filter);
	m_filter->set_visible_func(sigc::mem_fun(*this, &GtkTorrentTreeView::showMatches));
	get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

	rbuilder->get_widget("GtkMainWindow", m_parent);
	rbuilder->get_widget("infobar", m_infobar);

	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentView_onClick), false);

	signal_cursor_changed    ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onSelectionChanged ), false);
	signal_key_press_event   ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onKeyPress         ), false);

	set_model(m_liststore);
	setupColumns();
	set_enable_search();
	m_searchEntry = Gtk::manage(new Gtk::Entry());
	m_searchPopover = Gtk::manage(new Gtk::Popover());
	m_searchPopover->add(*m_searchEntry);
	m_searchPopover->set_relative_to(*this);
	m_searchPopover->set_modal();
	m_searchEntry->show();
	set_search_entry(*m_searchEntry);
	set_search_column(m_cols.m_col_name);
	set_hexpand();
	set_vexpand();
	reloadColors();

	std::vector<Gtk::TargetEntry> listTargets =
		{
			Gtk::TargetEntry("STRING"),
			Gtk::TargetEntry("text/plain"),
			Gtk::TargetEntry("text/uri-list"),
			Gtk::TargetEntry("application/x-bittorrent")
		};

	drag_dest_set(listTargets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE);
	signal_drag_data_received().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onFileDropped));
	get_selection()->unselect_all();

}

/**
* Sets up the actions for when tree view is clicked.
*/
bool GtkTorrentTreeView::torrentView_onClick(GdkEventButton *event)
{
	Gtk::TreeView::on_button_press_event(event);
	if(event->type == 5 && event->button == 1) //if double left click
		openView_onClick();

	if(event->button == 3) // if right-click
	{
		m_rcMenu                       = Gtk::manage(new Gtk::Menu());

		Gtk::MenuItem *rcmOpen        = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmOpenContainingFolder        = Gtk::manage(new Gtk::MenuItem("Open Containing Folder"));
		Gtk::SeparatorMenuItem *rcmSep0 = Gtk::manage(new Gtk::SeparatorMenuItem());

		Gtk::MenuItem *rcmCopyMagnetUrl        = Gtk::manage(new Gtk::MenuItem("Copy Magnet URL"));
		Gtk::MenuItem *rcmOpenUrlInBrowser        = Gtk::manage(new Gtk::MenuItem("Open URL In Browser"));
		Gtk::SeparatorMenuItem *rcmSep1 = Gtk::manage(new Gtk::SeparatorMenuItem());

		Gtk::MenuItem *rcmForceStart        = Gtk::manage(new Gtk::MenuItem("Force Start"));
		Gtk::MenuItem *rcmStart        = Gtk::manage(new Gtk::MenuItem("Start"));// TODO: Rename to Start depending on the state of the first selected item
		Gtk::MenuItem *rcmPause        = Gtk::manage(new Gtk::MenuItem("Pause"));
		Gtk::MenuItem *rcmStop        = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::SeparatorMenuItem *rcmSep2 = Gtk::manage(new Gtk::SeparatorMenuItem());

		Gtk::MenuItem *rcmRemove        = Gtk::manage(new Gtk::MenuItem("Remove"));
		Gtk::MenuItem *rcmRemoveAnd        = Gtk::manage(new Gtk::MenuItem("Remove And"));
		Gtk::SeparatorMenuItem *rcmSep3 = Gtk::manage(new Gtk::SeparatorMenuItem());

		Gtk::MenuItem *rcmForceReCheck        = Gtk::manage(new Gtk::MenuItem("Force Re-Check"));
		Gtk::MenuItem *rcmAdvanced        = Gtk::manage(new Gtk::MenuItem("Advanced"));
		Gtk::SeparatorMenuItem *rcmSep4 = Gtk::manage(new Gtk::SeparatorMenuItem());

		//Gtk::MenuItem *rcmPriority        = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		Gtk::MenuItem *rcmProperty        = Gtk::manage(new Gtk::MenuItem("Properties"));
		//rcmItemSeq                     = Gtk::manage(new Gtk::CheckMenuItem("Sequential Download"));

		rcmStart->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmStop->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmRemove->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmOpen->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));
		//rcmPriority->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmProperty->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));
		//rcmItemSeq->signal_realize().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::sequentialChange_onRealize));
		//rcmItemSeq->signal_toggled().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::sequentialChange_onClick));

		m_rcMenu->add(*rcmOpen);
		m_rcMenu->add(*rcmOpenContainingFolder);
		m_rcMenu->add(*rcmSep0);
		m_rcMenu->add(*rcmCopyMagnetUrl);
		m_rcMenu->add(*rcmOpenUrlInBrowser);
		m_rcMenu->add(*rcmSep1);

		m_rcMenu->add(*rcmForceStart);
		m_rcMenu->add(*rcmStart);
		m_rcMenu->add(*rcmPause);
		m_rcMenu->add(*rcmStop);
		m_rcMenu->add(*rcmSep2);

		m_rcMenu->add(*rcmRemove);
		m_rcMenu->add(*rcmRemoveAnd);
		m_rcMenu->add(*rcmSep3);

		m_rcMenu->add(*rcmForceReCheck);
		m_rcMenu->add(*rcmAdvanced);
		//m_rcMenu->add(*rcmPriority);
		m_rcMenu->add(*rcmSep4);

		m_rcMenu->add(*rcmProperty);
		//m_rcMenu->add(*rcmItemSeq);
		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}

	if(is_blank_at_pos(event->x, event->y) && event->send_event == false) // is_blank_at_pos return true even if the blank is the background of a row... so I just resend the click.
	{
		get_selection()->unselect_all();
		event->send_event = true; // why doesn't put() do that
		Gdk::Event((GdkEvent*)event).put();
	}
	m_infobar->updateInfo(getFirstSelected());
	onSelectionChanged();

	return true;
}

/**
* Sets up the action for when a mouse click is released on the torrent tree view.
*/
bool GtkTorrentTreeView::ColumnContextMenu_onRelease(GdkEventButton *event, Gtk::TreeViewColumn *tvc)
{
	tvc->set_visible(!tvc->get_visible());
	m_rcMenu->hide();
	return true;
}

bool GtkTorrentTreeView::torrentColumns_onClick(GdkEventButton *event)
{
	if(event->button == 3)
	{
		m_rcMenu = Gtk::manage(new Gtk::Menu());
		for(auto c : get_columns())
		{
			Gtk::CheckMenuItem *rcmStart = Gtk::manage(new Gtk::CheckMenuItem(c->get_title()));
			rcmStart->set_active(c->get_visible());
			rcmStart->signal_button_release_event().connect(sigc::bind<1>(sigc::mem_fun(*this, &GtkTorrentTreeView::ColumnContextMenu_onRelease), c));
			m_rcMenu->add(*rcmStart);
		}

		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}
	return true; //The bool that determine if the event has been handled allows to propagete or not a click
}

/**
* Sets up the columns in the torrent tree view.
*/
void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererProgress *cell = nullptr;

	if(gt::Settings::settings["ColumnsProperties"] != "")
		loadColumns();
	else
	{
		//append_column(      "Name", m_cols.m_col_name);
		cid = append_column(      "Name", m_cols.m_col_name);
		set_search_column(cid - 1);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_name);
		//append_column(         "#", m_cols.m_col_queue);
		cid = append_column(         "#", m_cols.m_col_queue);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_queue);
		//append_column(      "Size", m_cols.m_col_size);
		cid = append_column(      "Size", m_cols.m_col_bsize);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_size);
		append_column("Selected Size", m_cols.m_col_selected_size);
		append_column(   "Completed", m_cols.m_col_completed);
		append_column(   "Downloaded", m_cols.m_col_downloaded);
		//append_column(   "Remaining", m_cols.m_col_remaining);
		cid = append_column(   "Remaining", m_cols.m_col_bremaining);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_remaining);
		append_column(      "Done", m_cols.m_col_done);
		append_column(      "Status", m_cols.m_col_status);
		//append_column(      "Seeds", m_cols.m_col_seeds);
		cid = append_column(      "Seeds", m_cols.m_col_seeds);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_seeds);
		//append_column(     "Peers", m_cols.m_col_peers);
		cid = append_column(     "Peers", m_cols.m_col_peers);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_peers);
		append_column(     "Seeds/Peers", m_cols.m_col_seeds_peers);
		//append_column("Down Speed", m_cols.m_col_dl_speed);
		cid = append_column("Down Speed", m_cols.m_col_bdl_speed);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_dl_speed);
		//append_column(  "Up Speed", m_cols.m_col_ul_speed);
		cid = append_column(  "Up Speed", m_cols.m_col_bul_speed);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_ul_speed);
		cid = append_column(       "Age", m_cols.m_col_bage);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_age);
		//append_column(       "ETA", m_cols.m_col_eta);
		cid = append_column(       "ETA", m_cols.m_col_beta);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_eta);
		append_column(       "Uploaded", m_cols.m_col_uploaded);
		//append_column(     "Ratio", m_cols.m_col_dl_ratio);
		cid = append_column(     "Ratio", m_cols.m_col_dl_ratio);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_dl_ratio);
		append_column(      	"Avail.", m_cols.m_col_avail);
		append_column(       	 "Label", m_cols.m_col_label);
		append_column(    	"Added On", m_cols.m_col_added_on);
		append_column(	"Completed On", m_cols.m_col_completed_on);
		append_column(     	 "Tracker", m_cols.m_col_tracker);
		append_column(    	"Up Limit", m_cols.m_col_up_limit);
		append_column(  	"Down Limit", m_cols.m_col_down_limit);
		append_column(    	"Bw.Alloc", m_cols.m_col_bandwidth_allocation);
		append_column("Tracker Status", m_cols.m_col_tracker_status);
		append_column(       	 "Debug", m_cols.m_col_debug);
		append_column( 	 "Last Active", m_cols.m_col_last_active);
		append_column(     	 "Elapsed", m_cols.m_col_age);
		append_column(  	"Source URL", m_cols.m_col_source_url);
		append_column(     	 "Episode", m_cols.m_col_episode);
		append_column(      	"Format", m_cols.m_col_format);
		append_column(       	 "Codec", m_cols.m_col_codec);
	}

	for (auto & c : this->get_columns())
	{
		c->add_attribute(dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell())->property_background(), m_cols.m_col_background);
		c->add_attribute(dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell())->property_foreground(), m_cols.m_col_foreground);
	}

	cell = Gtk::manage(new Gtk::CellRendererProgress());
	cid = this->append_column("Progress", *cell);
	col = this->get_column(cid - 1);
	col->add_attribute(cell->property_value(), m_cols.m_col_percent);
	col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	col->add_attribute(cell->property_cell_background(), m_cols.m_col_background);
	col->set_sort_column(m_cols.m_col_percent);

	for (auto &c : this->get_columns())
	{
		Gtk::Button *butt = c->get_button();
		butt->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentColumns_onClick));
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		c->set_alignment(0.5f);
		c->set_clickable();
		c->set_resizable();
		c->set_reorderable();
		if(gt::Settings::settings["ColumnsProperties"] == "")
			c->set_fixed_width(128);
	}
	if(gt::Settings::settings["ColumnsProperties"] == "")
	{
		get_column(0) ->set_fixed_width(128);
		get_column(1) ->set_fixed_width(32);
		get_column(2) ->set_fixed_width(64);
		get_column(3) ->set_fixed_width(128);
		get_column(4) ->set_fixed_width(128);
		get_column(5) ->set_fixed_width(128);
		get_column(6) ->set_fixed_width(128);
		get_column(7) ->set_fixed_width(64);
		get_column(8) ->set_fixed_width(64);
		get_column(9) ->set_fixed_width(64);
		get_column(10)->set_fixed_width(64);
		get_column(11)->set_fixed_width(128);
		get_column(12)->set_fixed_width(128);
		get_column(13)->set_fixed_width(128);
		get_column(14)->set_fixed_width(64);
		get_column(15)->set_fixed_width(128);
		get_column(16)->set_fixed_width(64);
		get_column(17)->set_fixed_width(64);
		get_column(18)->set_fixed_width(64);
		get_column(19)->set_fixed_width(128);
		get_column(20)->set_fixed_width(128);
		get_column(21)->set_fixed_width(128);
		get_column(22)->set_fixed_width(128);
		get_column(23)->set_fixed_width(128);
		get_column(24)->set_fixed_width(128);
		get_column(25)->set_fixed_width(128);
		get_column(26)->set_fixed_width(64);
		get_column(27)->set_fixed_width(128);
		get_column(28)->set_fixed_width(128);
		get_column(29)->set_fixed_width(128);
		get_column(30)->set_fixed_width(128);
		get_column(31)->set_fixed_width(128);
	}
}

/**
* Sets up the cells in the torrent tree view.
*/
void GtkTorrentTreeView::addCell(std::shared_ptr<gt::Torrent> &t)
{
	if (t == NULL)
		return;

	Gtk::TreeModel::Row row      = *(m_liststore->append());
	// if there's a % in the state std::string, then the torrent is downloading
	std::string fgbg = t->getTextState().find('%') == std::string::npos ? t->getTextState() : "Downloading";

	row[m_cols.m_col_age]        = t->getTextActiveTime();
	row[m_cols.m_col_bage]       = t->getActiveTime();
	row[m_cols.m_col_beta]       = t->getEta();
	row[m_cols.m_col_eta]        = t->status().is_finished || t->status().is_seeding ? "" : t->getTextEta(); // TODO: replace with when dht is merged in core t->status().is_finished ? "" : t->getTextEta();
	row[m_cols.m_col_name]       = t->status().name;
	row[m_cols.m_col_seeders]    = t->status().num_seeds;
	row[m_cols.m_col_leechers]   = t->status().num_peers - t->status().num_seeds;
	row[m_cols.m_col_size]       = t->getTextSize();
	row[m_cols.m_col_seeds]      = t->getTotalSeeders();
	row[m_cols.m_col_peers]   	 = t->getTotalPeers();
	row[m_cols.m_col_eta]        = t->getTextEta();
	row[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
	row[m_cols.m_col_age]        = t->getTextActiveTime();
	row[m_cols.m_col_remaining]  = t->getTextRemaining();
	row[m_cols.m_col_bsize]      = t->getSize();
	row[m_cols.m_col_bremaining] = t->getRemaining();
	row[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
	row[m_cols.m_col_remaining]  = t->getTextRemaining();
	row[m_cols.m_col_background] =  m_colors[fgbg].first;
	row[m_cols.m_col_foreground] =  m_colors[fgbg].second;
	row[m_cols.m_col_torrent]    =  t;
}

/**
* Removes the cell in the torrent tree view.
*/
void GtkTorrentTreeView::removeCell(unsigned index)
{
//liststore->get_iter("0:" + std::to_string(index))
	m_liststore->erase(m_filter->convert_iter_to_child_iter(m_filter->get_iter("0:" + std::to_string(index))));
}

/**
* Updates the cells in the torrent tree view.
*/
void GtkTorrentTreeView::updateCells()
{
	int i = 0;
	for (auto & c : m_liststore->children())
	{
		std::shared_ptr<gt::Torrent> t = c[m_cols.m_col_torrent];

		std::string fgbg = t->getTextState().find('%') == std::string::npos ? t->getTextState() : "Downloading";
		c[m_cols.m_col_name]       = t->getName();
		c[m_cols.m_col_queue]      = i++;
		c[m_cols.m_col_size]       = t->getTextSize();
		//c[m_cols.m_col_selected_size]       =
		//c[m_cols.m_col_completed]       =
		//c[m_cols.m_col_downloaded]       =
		//c[m_cols.m_col_remaining]       =
		//c[m_cols.m_col_done]       =
		//c[m_cols.m_col_status]       =
		//c[m_cols.m_col_seeds]    = t->getTotalSeeders();
		//c[m_cols.m_col_peers]   = t->getTotalPeers();
		//c[m_cols.m_col_seeds_peers]       =
		//c[m_cols.m_col_dl_speed]   = t->getTextDownloadRate();
		//c[m_cols.m_col_ul_speed]   = t->getTextUploadRate();
		c[m_cols.m_col_age]        = t->getTextActiveTime();
		c[m_cols.m_col_bage]       = t->getActiveTime();
		c[m_cols.m_col_percent]    = t->getTotalProgress();
		c[m_cols.m_col_seeds]    = t->status().num_seeds;
		c[m_cols.m_col_peers]   = t->status().num_peers - t->status().num_seeds;
		c[m_cols.m_col_name]       = t->status().name;
		c[m_cols.m_col_ul_speed]   = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed]   = t->getTextDownloadRate();
		c[m_cols.m_col_bul_speed]  = t->getUploadRate();
		c[m_cols.m_col_bdl_speed]  = t->getDownloadRate();
		c[m_cols.m_col_size]       = t->getTextSize();
		c[m_cols.m_col_bsize]      = t->getSize();
		c[m_cols.m_col_beta]       = t->getTimeRemaining();
		c[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
		c[m_cols.m_col_eta]        = t->getHandle().status().is_finished|| t->getHandle().status().is_seeding ? "" : t->getTextEta(); // TODO: replace with when dht is merged in core t->status().is_finished ? "" : t->getTextEta();
		//c[m_cols.m_col_uploaded]       =
		c[m_cols.m_col_dl_ratio]   = t->getTextState();//
		//c[m_cols.m_col_avail]       =
		//c[m_cols.m_col_label]       =
		//c[m_cols.m_col_added_on]       =
		//c[m_cols.m_col_completed_on]       =
		//c[m_cols.m_col_tracker]       =
		//c[m_cols.m_col_up_limit]       =
		//c[m_cols.m_col_down_limit]       =
		//c[m_cols.m_col_bandwidth_allocation]       =
		//c[m_cols.m_col_tracker_status]       =
		//c[m_cols.m_col_debug]       =
		//c[m_cols.m_col_last_active]       =
		//c[m_cols.m_col_age]        = t->getTextActiveTime();
		//c[m_cols.m_col_source_url]       =
		//c[m_cols.m_col_episode]       =
		//c[m_cols.m_col_format]       =
		//c[m_cols.m_col_codec]       =
		//c[m_cols.m_col_percent]    = t->getTotalProgress();
		c[m_cols.m_col_background] = m_colors[fgbg].first;
		c[m_cols.m_col_foreground] = m_colors[fgbg].second;
	}
}

/**
* Gets the selected cells in the torrent tree view.
*/
std::vector<std::shared_ptr<gt::Torrent>> GtkTorrentTreeView::selectedTorrents()
{
	std::vector<std::shared_ptr<gt::Torrent>> torrents;

	Glib::RefPtr<Gtk::TreeSelection> sel = get_selection();
	std::vector<Gtk::TreeModel::Path> paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(get_model(), path));

	for (auto i : rows)
		torrents.push_back((*m_filter->get_iter(i.get_path()))[m_cols.m_col_torrent]);
	return torrents;
}

/**
* Gets the first selected cell in the torrent tree view.
*/
std::shared_ptr<gt::Torrent> GtkTorrentTreeView::getFirstSelected()
{
	if(selectedTorrents().empty())
		return nullptr;
	else
		return selectedTorrents()[0];
}

/**
* Pauses selected torrents in the torrent tree view.
*/
void GtkTorrentTreeView::setSelectedPaused(bool isPaused)
{
	for (auto i : selectedTorrents())
		i->setPaused(isPaused);// the pause button switches the status
}

/**
* Removes selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::removeSelected()
{
	Glib::RefPtr<Gtk::TreeSelection> sel = get_selection();
	std::vector<Gtk::TreeModel::Path> paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(get_model(), path));

	for (auto i : rows)
	{
		Gtk::TreeModel::iterator treeiter = m_liststore->get_iter(m_filter->convert_path_to_child_path(i.get_path()));
		Application::getSingleton()->getCore()->removeTorrent((*treeiter)[m_cols.m_col_torrent]);
		m_liststore->erase(treeiter);
	}
	m_parent->onSecTick();
	m_infobar->updateInfo(getFirstSelected());
}

/**
* Pauses selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::stopView_onClick()
{
	setSelectedPaused(true);
}

/**
* Opens the first selected torrent in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::openView_onClick()
{
	std::shared_ptr<gt::Torrent> t = getFirstSelected();

	if(t == nullptr || !t->status().has_metadata)
		return;

	gt::Platform::openTorrent(t);
}

/**
* Resumes the selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::startView_onClick()
{
	setSelectedPaused(false);
}

/**
* Removes selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::removeView_onClick()
{
	removeSelected();
}

/**
* Prioritizes selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::priorityView_onClick()
{
	/* Doesn't do nuffin wrong */
}

/**
* Launches properties for the firs selected torrent in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::propertyView_onClick()
{
	/* Doesn't do nuffin wrong */
}

/**
* Sets the first torrent selected in the torrent tree view using the right click menu to download sequentially.
*/
void GtkTorrentTreeView::sequentialChange_onClick()
{
	for (auto i : selectedTorrents())
		i->set_sequential_download(rcmItemSeq->get_active());
}

/**
* Prepares the drawing of the sequential download right click menu item according to the selected torrents.
*/
void GtkTorrentTreeView::sequentialChange_onRealize()
{
	if(!selectedTorrents().empty())
	{
		bool firstIsSeq = selectedTorrents()[0]->SequentialDownloadEnabled();
		for (auto i : selectedTorrents())
			if(i->SequentialDownloadEnabled() != firstIsSeq)
			{
				rcmItemSeq->set_inconsistent();
				return;
			}
		rcmItemSeq->set_active(firstIsSeq);
	}
	else
		rcmItemSeq->set_active(false);
}

void GtkTorrentTreeView::reloadColors()
{
	m_colors["Paused"                 ] = std::pair<std::string, std::string>(gt::Settings::settings["PausedForeGroundColor"        ], gt::Settings::settings["PausedBackGroundColor"        ]);
	m_colors["Queued for checking"    ] = std::pair<std::string, std::string>(gt::Settings::settings["QueuedCheckingForeGroundColor"], gt::Settings::settings["QueuedcheckingBackGroundColor"]);
	m_colors["Queued"                 ] = std::pair<std::string, std::string>(gt::Settings::settings["QueuedForeGroundColor"        ], gt::Settings::settings["QueuedBackGroundColor"        ]);
	m_colors["Downloading metadata..."] = std::pair<std::string, std::string>(gt::Settings::settings["MetadataForeGroundColor"      ], gt::Settings::settings["MetadataBackGroundColor"      ]);
	m_colors["Finished"               ] = std::pair<std::string, std::string>(gt::Settings::settings["FinishedForeGroundColor"      ], gt::Settings::settings["FinishedBackGroundColor"      ]);
	m_colors["Allocating..."          ] = std::pair<std::string, std::string>(gt::Settings::settings["AllocatingForeGroundColor"    ], gt::Settings::settings["AllocatingBackGroundColor"    ]);
	m_colors["Resuming..."            ] = std::pair<std::string, std::string>(gt::Settings::settings["ResumingForeGroundColor"      ], gt::Settings::settings["ResumingBackGroundColor"      ]);
	m_colors["Checking..."            ] = std::pair<std::string, std::string>(gt::Settings::settings["CheckingForeGroundColor"      ], gt::Settings::settings["CheckingBackGroundColor"      ]);
	m_colors["Seeding"                ] = std::pair<std::string, std::string>(gt::Settings::settings["SeedingForeGroundColor"       ], gt::Settings::settings["SeedingBackGroundColor"       ]);
	m_colors["Downloading"            ] = std::pair<std::string, std::string>(gt::Settings::settings["DownloadingForeGroundColor"   ], gt::Settings::settings["DownloadingBackGroundColor"   ]);
}

void GtkTorrentTreeView::onSelectionChanged(/*const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column*/)
{
	std::vector<std::shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	char pausedTorrents = 0, startedTorrents = 0;

	if(selectedTorrents().empty())
	{
		//m_parent->btn_pause ->hide();
		//m_parent->btn_resume->hide();
		return;
	}

	for (auto i : selectedTorrents())
	{
		pausedTorrents  +=  i->isPaused();
		startedTorrents += !i->isPaused();
		if(pausedTorrents && startedTorrents) break;
	}

	//m_parent->btn_pause ->set_visible(startedTorrents != 0);
	//m_parent->btn_resume->set_visible( pausedTorrents != 0);

	//m_parent->removeButton->set_visible(!selectedTorrents().empty());
	//m_parent->vSeparatorOne->set_visible(m_parent->removeButton->get_visible());
}

// columns are saved in a single settings, looking like this:
// ColumnsProperties = #|48|h,Age|120|v,Up Speed|120|v,
// Each element will be added from right to left.
// the list is formatted in that way:
// [Column0][Column1][Column...][ColumnN]
// Each column is formatted that way:
// [sTitle]|[iWidth]|[h],
// If the title is unknown, the whole element is ignored.
void GtkTorrentTreeView::saveColumns()
{
	std::string cStates;
	for(auto &c : get_columns())
		cStates += c->get_title() + '|' + std::to_string(c->get_width()) + '|' + ((c->get_visible()) ? 'v' : 'h') + ',';
	gt::Settings::settings["ColumnsProperties"] = cStates;
}

// This is where it gets tricky/ugly.
void GtkTorrentTreeView::loadColumns()
{
	std::vector<std::string> titles = { "#", "Age", "ETA", "Name", "Seed", "Leech", "Up Speed", "Down Speed", "Size", "Remains", "Ratio" };
	std::vector<Gtk::TreeModelColumnBase*> cols
	{
		&m_cols.m_col_queue,
		&m_cols.m_col_age,
		&m_cols.m_col_eta,
		&m_cols.m_col_name,
		&m_cols.m_col_seeds,
		&m_cols.m_col_peers,
		&m_cols.m_col_ul_speed,
		&m_cols.m_col_dl_speed,
		&m_cols.m_col_size,
		&m_cols.m_col_remaining,
		&m_cols.m_col_dl_ratio
	};

	std::vector<Gtk::TreeModelColumnBase*> scols
	{
		&m_cols.m_col_queue,
		&m_cols.m_col_bage,
		&m_cols.m_col_beta,
		&m_cols.m_col_name,
		&m_cols.m_col_seeds,
		&m_cols.m_col_peers,
		&m_cols.m_col_bul_speed,
		&m_cols.m_col_bdl_speed,
		&m_cols.m_col_bsize,
		&m_cols.m_col_bremaining,
		&m_cols.m_col_dl_ratio
	};

	std::string tmp = gt::Settings::settings["ColumnsProperties"];
	if (tmp == "")
		tmp = "#|20|h,Age|50|h,ETA|90|v,Name|250|v,Seed|45|v,Leech|45|v,Up Speed|95|v,Down Speed|95|v,Size|75|v,Remains|75|h,Ratio|55|h,Progress|160|v,";

	do
	{
		std::string title = tmp.substr(0, tmp.find('|'));
		tmp = tmp.substr(tmp.find('|') + 1);
		int width = stoi(tmp.substr(0, tmp.find('|')));
		tmp = tmp.substr(tmp.find('|') + 1);
		bool hidden = tmp.substr(0, tmp.find(','))[0] == 'h';
		tmp = tmp.substr(tmp.find(',') + 1);
		int index = find(titles.begin(), titles.end(), title) - titles.begin();
		if(index == 11) continue;
		else
		{
			auto k = (index == 1 || index == 5 || index == 6)
			         ? get_column(append_column(titles[index], *static_cast<Gtk::TreeModelColumn<unsigned>     *>(cols[index])) - 1)
			         : get_column(append_column(titles[index], *static_cast<Gtk::TreeModelColumn<Glib::ustring>*>(cols[index])) - 1);
			k->set_fixed_width(width);
			k->set_visible(!hidden);
			k->set_sort_column(*scols[index]);
		}
	}
	while (tmp != "");
}

bool GtkTorrentTreeView::onKeyPress(GdkEventKey *event)
{
	m_infobar->updateInfo(getFirstSelected());
	short arrowkeys[] = { 80, 88, 83, 85, 111, 114, 113, 116 };
	if(std::find(arrowkeys, arrowkeys + 8, event->hardware_keycode) == arrowkeys + 8) return false;
	m_infobar->updateInfo(getFirstSelected());
	if(event->send_event) return true;
	event->send_event = true;
	Gdk::Event((GdkEvent*)event).put();
	return false;
}

bool GtkTorrentTreeView::showMatches(const Gtk::TreeModel::const_iterator& iter)
{
	if(!get_search_entry() || get_search_entry()->get_text() == "") return true; //show every rows if no text has been entered

	std::string toMatch = iter->get_value(m_cols.m_col_name);
	std::string matching(this->get_search_entry()->get_text());

	std::transform(toMatch.begin(), toMatch.end(), toMatch.begin(), tolower);
	std::transform(matching.begin(), matching.end(), matching.begin(), tolower);

	Glib::PatternSpec matcher("*" + matching + "*");

	return matcher.match(toMatch);
}

void GtkTorrentTreeView::onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	std::string sel_data = selection_data.get_data_as_string();
	if(Application::getSingleton()->getCore()->isLink(sel_data))
	{
		std::shared_ptr<gt::Torrent> t = Application::getSingleton()->getCore()->addTorrent(sel_data);
		if (t)//Checks if t is not null
		{
			t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, m_parent, std::placeholders::_1, std::placeholders::_2);
			addCell(t);
		}
	}
	else
	{
		std::string fn = Glib::filename_from_uri(sel_data);
		//I'm not even sure why we needed to trim the filename
		//boost::algorithm::trim(fn); //d-don't worry guys! w-we only need boo-boost for libtorrent! th-that's all!
		fn.erase(0, fn.find_first_not_of(' '));
		fn.erase(fn.find_last_not_of(' ') + 1);

		bool want_uncertain = true;
		std::string content_type = Gio::content_type_guess(fn, sel_data, want_uncertain);
		if(content_type == "application/x-bittorrent" || content_type == ".torrent")
		{
			std::shared_ptr<gt::Torrent> t = Application::getSingleton()->getCore()->addTorrent(fn);
			if (t)//Checks if t is not null
			{
				t->onStateChanged = std::bind(&GtkMainWindow::torrentStateChangedCallback, m_parent, std::placeholders::_1, std::placeholders::_2);
				addCell(t);
			}
			//TODO Add error dialogue if torrent add is unsuccessful
		}
	}
}
