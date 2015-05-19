#include <cctype>

#include <glibmm/pattern.h>
#include <gtkmm/treerowreference.h>
#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/separatormenuitem.h>

#include <gtorrent/Log.hpp>
#include <gtorrent/Settings.hpp>
#include <gtorrent/Platform.hpp>

#include "../../Application.hpp"
#include "../GtkMainWindow.hpp"

/**
* Sets up the tree view containing torrent information.
*/
GtkTorrentTreeView::GtkTorrentTreeView(GtkTreeView *treeview, const Glib::RefPtr<Gtk::Builder> rbuilder) :
	Gtk::TreeView(treeview),
	m_builder(rbuilder)
{
	m_liststore = Gtk::ListStore::create(m_cols);
	m_filter = Gtk::TreeModelFilter::create(m_liststore);
	m_filtersort = Gtk::TreeModelSort::create(m_filter);
	m_filter->set_visible_func(sigc::mem_fun(*this, &GtkTorrentTreeView::showMatches));

	get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

	rbuilder->get_widget("GtkMainWindow", m_parent); // Nyanpasu: Maybe m_parent isn't even needed since widgets store parents.
	rbuilder->get_widget_derived("torrent_infobar", m_infobar);
	m_searchEntry = Gtk::manage(new Gtk::Entry());
	m_searchPopover = Gtk::manage(new Gtk::Popover());

	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onClickTree), false);
	signal_cursor_changed().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onSelectionChanged), false);
	signal_key_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onKeyPress), false);

	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_queue"))     ->pack_start(m_cols.m_col_queue);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_name"))      ->pack_start(m_cols.m_col_name);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_eta"))       ->pack_start(m_cols.m_col_eta);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_age"))       ->pack_start(m_cols.m_col_age);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_seeders"))   ->pack_start(m_cols.m_col_seeders);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_leechers"))  ->pack_start(m_cols.m_col_leechers);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_upload"))    ->pack_start(m_cols.m_col_ul_speed);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_download"))  ->pack_start(m_cols.m_col_dl_speed);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_size"))      ->pack_start(m_cols.m_col_size);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_remaining")) ->pack_start(m_cols.m_col_remaining);
	Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_ratio"))     ->pack_start(m_cols.m_col_dl_ratio);

	if(gt::Settings::settings["ColumnsProperties"] != "")
		loadColumns();
	else
		setupColumns();

	// Set up renderer for progress bar. Exception because setupColumns() only handle text renderers by default.
	Glib::RefPtr<Gtk::TreeViewColumn> col = Glib::RefPtr<Gtk::TreeViewColumn>::cast_static(m_builder->get_object("torrent_col_progress"));
	Gtk::CellRendererProgress *cell = Gtk::manage(new Gtk::CellRendererProgress());
	col->pack_start(*cell);
	// Maybe we can just use set_renderer?
	// Last time I tried I got a cray cray backtrace and stack overflow.
	// TODO Look into that ^
	col->add_attribute(cell->property_value(), m_cols.m_col_percent);
	col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	col->add_attribute(cell->property_cell_background(), m_cols.m_col_background);
	col->set_sort_column(m_cols.m_col_percent);

	set_enable_search();
	set_search_entry(*m_searchEntry);
	set_search_column(m_cols.m_col_name);

	m_searchPopover->add(*m_searchEntry);
	m_searchPopover->set_relative_to(*this);
	m_searchPopover->set_modal();
	m_searchEntry->show();

	//set_model(m_filtersort);
	set_model(m_liststore);

	reloadColors();

        // Set drag targets
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
bool GtkTorrentTreeView::onClickTree(GdkEventButton *event)
{
	Gtk::TreeView::on_button_press_event(event);
	if(event->type == 5 && event->button == 1) //if double left click
		openView_onClick();

	if(event->button == 3) // if right-click
	{
		// TODO Make menu from builder.
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
 * Hide context menu
 */
bool GtkTorrentTreeView::onReleaseMenu(GdkEventButton *event, Gtk::TreeViewColumn *tvc)
{
	tvc->set_visible(!tvc->get_visible());
	m_rcMenu->hide();
	return true;
}

bool GtkTorrentTreeView::onClickColumns(GdkEventButton *event)
{
	if(event->button == 3)
	{
		m_rcMenu = Gtk::manage(new Gtk::Menu());
		for(auto c : get_columns())
		{
			Gtk::CheckMenuItem *rcmItem1 = Gtk::manage(new Gtk::CheckMenuItem(c->get_title()));
			rcmItem1->set_active(c->get_visible());
			rcmItem1->signal_button_release_event().connect(sigc::bind<1>(
				sigc::mem_fun(*this, &GtkTorrentTreeView::onReleaseMenu), c));
			m_rcMenu->add(*rcmItem1);
		}

		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}
	return true; //The bool that determine if the event has been handled allows to propagete or not a click
}

/**
 * Iterate through every column and set default settings for the column
 * TODO Consider setting these properties for each column in there resource files
 *      Find out if it's posssible to set the renderers in the resources too
 */
void GtkTorrentTreeView::setupColumns()
{
	for (auto &c : this->get_columns())
	{
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		c->set_alignment(0.5f);
		c->set_clickable();
		c->set_resizable();
		c->set_reorderable();

		Gtk::CellRendererText *r = dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell());
		if (!r)
			continue;
		c->add_attribute(r->property_background(), m_cols.m_col_background);
		c->add_attribute(r->property_foreground(), m_cols.m_col_foreground);

		Gtk::Button *butt = c->get_button();
		butt->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onClickColumns));
	}
}

/**
* Add a row to the torrent tree view.
*/
void GtkTorrentTreeView::addCell(std::shared_ptr<gt::Torrent> &t)
{
	if (!t)
		return;

	Gtk::TreeModel::Row row      = *(m_liststore->append());
	row[m_cols.m_col_torrent] = t;
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

		// if there's a % in the state std::string, then the torrent is downloading
		// 10/10 logic
		std::string fgbg = t->getTextState().find('%') == std::string::npos ? t->getTextState() : "Downloading";

		c[m_cols.m_col_bremaining ] = t->status().total_wanted - t->status().total_download;
		c[m_cols.m_col_queue      ] = i++;
		c[m_cols.m_col_age        ] = t->getTextActiveTime();
		c[m_cols.m_col_bage       ] = t->status().active_time;
		c[m_cols.m_col_percent    ] = t->getTotalProgress();
		c[m_cols.m_col_seeders    ] = t->status().num_seeds;
		c[m_cols.m_col_leechers   ] = t->status().num_peers - t->status().num_seeds;
		c[m_cols.m_col_name       ] = t->status().name;
		c[m_cols.m_col_ul_speed   ] = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed   ] = t->getTextDownloadRate();
		c[m_cols.m_col_bul_speed  ] = t->getUploadRate();
		c[m_cols.m_col_bdl_speed  ] = t->getDownloadRate();
		c[m_cols.m_col_size       ] = t->getTextSize();
		c[m_cols.m_col_bsize      ] = t->status().total_wanted;
		c[m_cols.m_col_beta       ] = (t->getDownloadRate() > 0) ? t->status().total_wanted / t->getDownloadRate() : 0;
		c[m_cols.m_col_dl_ratio   ] = t->getTextTotalRatio();
		c[m_cols.m_col_eta        ] = t->status().is_finished|| t->status().is_seeding ? "" : t->getTextEta(); // TODO: replace with when dht is merged in core t->status().is_finished ? "" : t->getTextEta();
		c[m_cols.m_col_background ] = m_colors[fgbg] .first;
		c[m_cols.m_col_foreground ] = m_colors[fgbg] .second;
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
	bool pausedTorrents = false, startedTorrents = false;

	if(selectedTorrents().empty())
	{
		m_parent->pauseButton  ->hide();
		m_parent->resumeButton ->hide();
		m_parent->removeButton ->hide();
		m_parent->vSeparatorOne->hide();
		return;
	}

	for (auto i : selectedTorrents())
	{
		pausedTorrents  = pausedTorrents  ? pausedTorrents  : i->isPaused();
		startedTorrents = startedTorrents ? startedTorrents : !i->isPaused();

		if(pausedTorrents && startedTorrents) break;
	}

	m_parent->pauseButton ->set_visible(startedTorrents);
	m_parent->resumeButton->set_visible( pausedTorrents);

	m_parent->removeButton->set_visible(!selectedTorrents().empty());
	m_parent->vSeparatorOne->set_visible();
}

/*
 * Save column properties to settings
 *
 * columns are saved in a single settings, looking like this:
 * ColumnsProperties = #|48|h,Age|120|v,Up Speed|120|v,
 * Each element will be added from right to left.
 * the list is formatted in that way:
 * [Column0][Column1][Column...][ColumnN]
 * Each column is formatted that way:
 * [sTitle]|[iWidth]|[h],
 * If the title is unknown, the whole element is ignored.
 */
void GtkTorrentTreeView::saveColumns()
{
	std::string cStates;
	for(auto &c : get_columns())
		cStates += c->get_title() + '|' + std::to_string(c->get_width()) + '|' + ((c->get_visible()) ? 'v' : 'h') + ',';
	gt::Settings::settings["ColumnsProperties"] = cStates;
}

/**
 * Load column properties from settings
 * TODO Modify so that it hides columns instead of not loading them at all.
 * TODO Refactor and enable. Currently disabled and delegates to setupColumns() instead
 *
 */
void GtkTorrentTreeView::loadColumns()
{
	setupColumns();
	return;
/*
	std::string tmp = gt::Settings::settings["ColumnsProperties"];
	if (tmp == "")
		tmp = "#|20|h,Age|50|h,ETA|90|v,Name|250|v,Seed|45|v,Leech|45|v,Up Speed|95|v,Down Speed|95|v,Size|75|v,Remains|75|h,Ratio|55|h,Progress|160|v,";

	// TODO Refactor
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
 */
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

void GtkTorrentTreeView::onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	std::string sel_data = selection_data.get_data_as_string();
	if(Application::getSingleton()->getCore()->isLink(sel_data))
	{
		std::shared_ptr<gt::Torrent> t = Application::getSingleton()->getCore()->addTorrent(sel_data);
		if (t)
		{
			t->onStateChanged = std::bind(&GtkTorrentBox::onTorrentStateChange, m_parent->m_box_torrent, std::placeholders::_1, std::placeholders::_2);
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
				t->onStateChanged = std::bind(&GtkTorrentBox::onTorrentStateChange, m_parent->m_box_torrent, std::placeholders::_1, std::placeholders::_2);
				addCell(t);
			}
			//TODO Add error dialogue if torrent add is unsuccessful
		}
	}

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
