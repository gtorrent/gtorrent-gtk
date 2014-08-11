#include "GtkTorrentTreeView.hpp"
#include "GtkMainWindow.hpp"
#include <giomm/file.h>
#include <gtkmm/separatormenuitem.h>
#include <Log.hpp>
#include <Settings.hpp>
#include <Platform.hpp>

/**
* Sets up the tree view containing torrent information.
*/
GtkTorrentTreeView::GtkTorrentTreeView(GtkMainWindow *Parent, GtkTorrentInfoBar *InfoBar) : m_infobar(InfoBar), m_parent(Parent)
{
	m_liststore = Gtk::ListStore::create(m_cols);
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentView_onClick), false);
	signal_cursor_changed().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::onSelectionChanged), false);
	set_model(m_liststore);
	setupColumns();
	set_hexpand();
	set_vexpand();
	reloadColors();
	for(auto tor : Application::getSingleton()->getCore()->getTorrents())
		addCell(tor);
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
		Gtk::MenuItem *rcmItem1        = Gtk::manage(new Gtk::MenuItem("Start"));
		Gtk::MenuItem *rcmItem2        = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::MenuItem *rcmItem3        = Gtk::manage(new Gtk::MenuItem("Remove"));
		Gtk::SeparatorMenuItem *rcSep1 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem4        = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5        = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		Gtk::SeparatorMenuItem *rcSep2 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem6        = Gtk::manage(new Gtk::MenuItem("Property"));
		rcmItemSeq                     = Gtk::manage(new Gtk::CheckMenuItem("Sequential Download"));

		rcmItem1->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmItem2->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmItem3->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmItem4->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));

		/* Maybe an onHover or smth for this one. */
		rcmItem5->signal_activate() .connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmItem6->signal_activate() .connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));
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
		append_column(         "#", m_cols.m_col_queue);
		append_column(       "Age", m_cols.m_col_age);
		append_column(       "ETA", m_cols.m_col_eta);
		append_column(      "Name", m_cols.m_col_name);
		append_column(      "Seed", m_cols.m_col_seeders);
		append_column(     "Leech", m_cols.m_col_leechers);
		append_column(  "Up Speed", m_cols.m_col_ul_speed);
		append_column("Down Speed", m_cols.m_col_dl_speed);
		append_column(      "Size", m_cols.m_col_size);
		append_column(   "Remains", m_cols.m_col_remaining);
		append_column(     "Ratio", m_cols.m_col_dl_ratio);
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
	this->get_column(0)->set_fixed_width(48);
}

/**
* Sets up the cells in the torrent tree view.
*/
void GtkTorrentTreeView::addCell(shared_ptr<gt::Torrent> &t)
{
	if (t == NULL)
		return;

	Gtk::TreeModel::Row row      = *(m_liststore->append());
	// if there's a % in the state string, then the torrent is downloading
	string fgbg = t->getTextState().find('%') == string::npos ? t->getTextState() : "Downloading";

	row[m_cols.m_col_age]        = t->getTextActiveTime();
	row[m_cols.m_col_eta]        = t->getTextEta();
	row[m_cols.m_col_name]       = t->getName();
	row[m_cols.m_col_seeders]    = t->getTotalSeeders();
	row[m_cols.m_col_leechers]   = t->getTotalLeechers();
	row[m_cols.m_col_size]       = t->getTextSize();
	row[m_cols.m_col_remaining]  = t->getTextRemaining();
	row[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
	row[m_cols.m_col_background] =  m_colors[fgbg].first;
	row[m_cols.m_col_foreground] =  m_colors[fgbg].second;

}

/**
* Removes the cell in the torrent tree view.
*/
void GtkTorrentTreeView::removeCell(unsigned index)
{
	stringstream strIndex;
	strIndex << "0:" << index;
	m_liststore->erase(m_liststore->get_iter(strIndex.str()));
}

/**
* Updates the cells in the torrent tree view.
*/
void GtkTorrentTreeView::updateCells()
{
	unsigned int i = 0;
	for (auto & c : m_liststore->children())
	{
		shared_ptr<gt::Torrent> t = Application::getSingleton()->getCore()->getTorrents()[i];
		string fgbg = t->getTextState().find('%') == string::npos ? t->getTextState() : "Downloading";

		c[m_cols.m_col_age]        = t->getTextActiveTime();
		c[m_cols.m_col_percent]    = t->getTotalProgress();
		c[m_cols.m_col_seeders]    = t->getTotalSeeders();
		c[m_cols.m_col_leechers]   = t->getTotalLeechers();
		c[m_cols.m_col_name]       = t->getName();
		c[m_cols.m_col_ul_speed]   = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed]   = t->getTextDownloadRate();
		c[m_cols.m_col_size]       = t->getTextSize();
		c[m_cols.m_col_dl_ratio]   = t->getTextState();
		c[m_cols.m_col_eta]        = t->getTextTimeRemaining();
		c[m_cols.m_col_background] = m_colors[fgbg].first;
		c[m_cols.m_col_foreground] = m_colors[fgbg].second;

// TODO: Handle with events

		//m_cells[i]->property_text() = t->getTextState();

		++i;
	}

}

/**
* Gets the selected cells in the torrent tree view.
*/
vector<unsigned> GtkTorrentTreeView::selectedIndices()
{
	Glib::RefPtr<Gtk::TreeSelection> sel = this->get_selection();
	sel->set_mode(Gtk::SelectionMode::SELECTION_MULTIPLE);
	vector<Gtk::TreeModel::Path> path = sel->get_selected_rows();
	vector<unsigned> indices;
	for (auto val : path)
		indices.push_back(val[0]); // we only get the first index because our tree is 1 node deep
	return indices;
}

/**
* Gets the first selected cell in the torrent tree view.
*/
shared_ptr<gt::Torrent> GtkTorrentTreeView::getFirstSelected()
{
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	if(selectedIndices().size() < 1)
		return nullptr;
	else
		return t[selectedIndices()[0]];
}

/**
* Pauses selected torrents in the torrent tree view.
*/
void GtkTorrentTreeView::setSelectedPaused(bool isPaused)
{
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for (auto i : selectedIndices())
		t[i]->setPaused(isPaused);// the pause button switches the status

}

/**
* Removes selected torrents in the torrent tree view using the right click menu.
*/
void GtkTorrentTreeView::removeSelected()
{
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for (auto i : selectedIndices())
	{
		Application::getSingleton()->getCore()->removeTorrent(t[i]);
		removeCell(i);
	}
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
	shared_ptr<gt::Torrent> t = getFirstSelected();

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
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();

	for (auto i : selectedIndices())
		t[i]->setSequentialDownload(rcmItemSeq->get_active());
}

/**
* Prepares the drawing of the sequential download right click menu item according to the selected torrents.
*/
void GtkTorrentTreeView::sequentialChange_onRealize()
{
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();

	if(selectedIndices().size() > 0)
	{
		bool firstIsSeq = t[selectedIndices()[0]]->SequentialDownloadEnabled();
		for (auto i : selectedIndices())
			if(t[i]->SequentialDownloadEnabled() != firstIsSeq)
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
	m_colors["Paused"]                  = pair<string, string>(gt::Settings::settings["PausedForeGroundColor"],      gt::Settings::settings["PausedBackGroundColor"]);
	m_colors["Queued for checking"]     = pair<string, string>(gt::Settings::settings["QueuedForeGroundColor"],      gt::Settings::settings["QueuedcheckingBackGroundColor"]);
	m_colors["Downloading metadata..."] = pair<string, string>(gt::Settings::settings["MetadataForeGroundColor"],    gt::Settings::settings["MetadataBackGroundColor"]);
	m_colors["Finished"]                = pair<string, string>(gt::Settings::settings["FinishedForeGroundColor"],    gt::Settings::settings["FinishedBackGroundColor"]);
	m_colors["Allocating..."]           = pair<string, string>(gt::Settings::settings["AllocatingForeGroundColor"],  gt::Settings::settings["AllocatingBackGroundColor"]);
	m_colors["Resuming..."]             = pair<string, string>(gt::Settings::settings["ResumingForeGroundColor"],    gt::Settings::settings["ResumingBackGroundColor"]);
	m_colors["Checking..."]             = pair<string, string>(gt::Settings::settings["CheckingForeGroundColor"],    gt::Settings::settings["CheckingBackGroundColor"]);
	m_colors["Seeding"]                 = pair<string, string>(gt::Settings::settings["SeedingForeGroundColor"],     gt::Settings::settings["SeedingBackGroundColor"]);
	m_colors["Downloading"]             = pair<string, string>(gt::Settings::settings["DownloadingForeGroundColor"], gt::Settings::settings["DownloadingBackGroundColor"]);

}

void GtkTorrentTreeView::onSelectionChanged(/*const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column*/)
{
	vector<shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	char pausedTorrents = 0, startedTorrents = 0;

	if(selectedIndices().empty())
	{
		m_parent->btn_pause ->hide();
		m_parent->btn_resume->hide();
		return;
	}

	for (auto i : selectedIndices())
	{
		pausedTorrents  +=  t[i]->isPaused();
		startedTorrents += !t[i]->isPaused();
		if(pausedTorrents && startedTorrents) break;
	}

	m_parent->btn_pause ->set_visible(startedTorrents != 0);
	m_parent->btn_resume->set_visible( pausedTorrents != 0);

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
	string cStates;
	for(auto &c : get_columns())
		cStates += c->get_title() + '|' + to_string(c->get_width()) + '|' + ((c->get_visible()) ? 'v' : 'h') + ',';
	gt::Settings::settings["ColumnsProperties"] = cStates;
}

// This is where it gets tricky/ugly.
void GtkTorrentTreeView::loadColumns()
{
	vector<string> titles = { "#", "Age", "ETA", "Name", "Seed", "Leech", "Up Speed", "Down Speed", "Size", "Remains", "Ratio" };
	vector<Gtk::TreeModelColumnBase*> cols
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
	string tmp = gt::Settings::settings["ColumnsProperties"];
	do
	{
		string title = tmp.substr(0, tmp.find('|'));
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
		}
	} while (tmp != "");
}
