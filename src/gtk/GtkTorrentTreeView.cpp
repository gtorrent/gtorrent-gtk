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
GtkTorrentTreeView::GtkTorrentTreeView(GtkMainWindow *Parent, GtkTorrentInfoBar *InfoBar) : m_infobar(InfoBar), m_parent(Parent)
{
	m_liststore = Gtk::ListStore::create(m_cols);
	m_filter = Gtk::TreeModelFilter::create(m_liststore);
	m_filtersort = Gtk::TreeModelSort::create(m_filter);
	m_filter->set_visible_func(sigc::mem_fun(*this, &GtkTorrentTreeView::showMatches));
	get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentView_onClick), false);
	signal_cursor_changed().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onSelectionChanged), false);
	signal_key_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onKeyPress), false);
	set_model(m_filtersort);
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
		Gtk::MenuItem *rcmItem1        = Gtk::manage(new Gtk::MenuItem("Start"));// TODO: Rename to Start depending on the state of the first selected item
		Gtk::MenuItem *rcmItem2        = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::MenuItem *rcmItem3        = Gtk::manage(new Gtk::MenuItem("Remove"));
		Gtk::SeparatorMenuItem *rcSep1 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem4        = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5        = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		Gtk::SeparatorMenuItem *rcSep2 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem6        = Gtk::manage(new Gtk::MenuItem("Property"));
		rcmItemSeq                     = Gtk::manage(new Gtk::CheckMenuItem("Sequential Download"));

		rcmItem1->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmItem2->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmItem3->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmItem4->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));
		rcmItem5->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmItem6->signal_activate ().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));
		rcmItemSeq->signal_realize().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::sequentialChange_onRealize));
		rcmItemSeq->signal_toggled().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::sequentialChange_onClick));

		m_rcMenu->add(*rcmItem1);
		m_rcMenu->add(*rcmItem2);
		m_rcMenu->add(*rcmItem3);
		m_rcMenu->add(*rcSep1);
		m_rcMenu->add(*rcmItem4);
		m_rcMenu->add(*rcmItem5);
		m_rcMenu->add(*rcSep2);
		m_rcMenu->add(*rcmItem6);
		m_rcMenu->add(*rcmItemSeq);
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
			Gtk::CheckMenuItem *rcmItem1 = Gtk::manage(new Gtk::CheckMenuItem(c->get_title()));
			rcmItem1->set_active(c->get_visible());
			rcmItem1->signal_button_release_event().connect(sigc::bind<1>(sigc::mem_fun(*this, &GtkTorrentTreeView::ColumnContextMenu_onRelease), c));
			m_rcMenu->add(*rcmItem1);
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
		cid = append_column(         "#", m_cols.m_col_queue);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_queue);
		cid = append_column(       "Age", m_cols.m_col_bage);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_age);
		cid = append_column(       "ETA", m_cols.m_col_beta);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_eta);
		cid = append_column(      "Name", m_cols.m_col_name);
		set_search_column(cid - 1);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_name);
		cid = append_column(      "Seed", m_cols.m_col_seeders);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_seeders);
		cid = append_column(     "Leech", m_cols.m_col_leechers);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_leechers);
		cid = append_column(  "Up Speed", m_cols.m_col_bul_speed);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_ul_speed);
		cid = append_column("Down Speed", m_cols.m_col_bdl_speed);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_dl_speed);
		cid = append_column(      "Size", m_cols.m_col_bsize);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_size);
		cid = append_column(   "Remains", m_cols.m_col_bremaining);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_remaining);
		cid = append_column(     "Ratio", m_cols.m_col_dl_ratio);
		col = get_column(cid - 1);
		col->set_sort_column(m_cols.m_col_dl_ratio);
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
			c->set_fixed_width(120);
	}
	if(gt::Settings::settings["ColumnsProperties"] == "")
		get_column(0)->set_fixed_width(48);
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
	row[m_cols.m_col_eta]        = t->getHandle().status().is_finished|| t->getHandle().status().is_seeding ? "" : t->getTextEta(); // TODO: replace with when dht is merged in core t->status().is_finished ? "" : t->getTextEta();
	row[m_cols.m_col_name]       = t->getName();
	row[m_cols.m_col_seeders]    = t->getTotalSeeders();
	row[m_cols.m_col_leechers]   = t->getTotalLeechers();
	row[m_cols.m_col_size]       = t->getTextSize();
	row[m_cols.m_col_remaining]  = t->getTextRemaining();
	row[m_cols.m_col_bsize]      = t->getSize();
	row[m_cols.m_col_bremaining] = t->getRemaining();
	row[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
	row[m_cols.m_col_background] = m_colors[fgbg].first;
	row[m_cols.m_col_foreground] = m_colors[fgbg].second;
	row[m_cols.m_col_torrent]    = t;
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

		c[m_cols.m_col_queue]      = i++;
		c[m_cols.m_col_age]        = t->getTextActiveTime();
		c[m_cols.m_col_bage]       = t->getActiveTime();
		c[m_cols.m_col_percent]    = t->getTotalProgress();
		c[m_cols.m_col_seeders]    = t->getTotalSeeders();
		c[m_cols.m_col_leechers]   = t->getTotalLeechers();
		c[m_cols.m_col_name]       = t->getName();
		c[m_cols.m_col_ul_speed]   = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed]   = t->getTextDownloadRate();
		c[m_cols.m_col_bul_speed]  = t->getUploadRate();
		c[m_cols.m_col_bdl_speed]  = t->getDownloadRate();
		c[m_cols.m_col_size]       = t->getTextSize();
		c[m_cols.m_col_bsize]      = t->getSize();
		c[m_cols.m_col_beta]       = t->getTimeRemaining();
		c[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
		c[m_cols.m_col_eta]        = t->getHandle().status().is_finished|| t->getHandle().status().is_seeding ? "" : t->getTextEta(); // TODO: replace with when dht is merged in core t->status().is_finished ? "" : t->getTextEta();
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

	if(t == nullptr || !t->hasMetadata())
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
		i->setSequentialDownload(rcmItemSeq->get_active());
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
		m_parent->btn_pause ->hide();
		m_parent->btn_resume->hide();
		return;
	}

	for (auto i : selectedTorrents())
	{
		pausedTorrents  +=  i->isPaused();
		startedTorrents += !i->isPaused();
		if(pausedTorrents && startedTorrents) break;
	}

	m_parent->btn_pause ->set_visible(startedTorrents != 0);
	m_parent->btn_resume->set_visible( pausedTorrents != 0);

	m_parent->btn_remove->set_visible(!selectedTorrents().empty());
	m_parent->separator1->set_visible(m_parent->btn_remove->get_visible());
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
		&m_cols.m_col_seeders,
		&m_cols.m_col_leechers,
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
		&m_cols.m_col_seeders,
		&m_cols.m_col_leechers,
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
