#include "GtkTorrentTreeView.hpp"

#include <giomm/file.h>
#include <gtkmm/separatormenuitem.h>
/**
* Sets up the tree view containing torrent information.
*/
GtkTorrentTreeView::GtkTorrentTreeView(GtkTorrentInfoBar *InfoBar) : m_infobar(InfoBar)
{
	m_liststore = Gtk::ListStore::create(m_cols);
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentView_onClick), false);

	this->set_model(m_liststore);
	this->setupColumns();

	m_colors["Paused"]                  = pair<string, string>("#ff00ff", "#00ff00");
	m_colors["Queued for checking"]     = pair<string, string>("#ff00ff", "#00ff00");
	m_colors["Downloading metadata..."] = pair<string, string>("#ffff00", "#0000ff");
	m_colors["Finished"]                = pair<string, string>("#0f0f0f", "#f0f0f0");
	m_colors["Allocating..."]           = pair<string, string>("#ff0ff0", "#00f00f");
	m_colors["Resuming..."]             = pair<string, string>("#00ffff", "#ff0000");
	m_colors["Checking..."]             = pair<string, string>("#f00f0f", "#0ff0f0");
	m_colors["Seeding"]                 = pair<string, string>("#123456", "#789ABC");
	m_colors["Downloading"]             = pair<string, string>("#00fe00", "#789ABC");
	
	for(auto tor : Application::getSingleton()->getCore()->getTorrents())
		addCell(tor);
}

/**
* Sets up the actions for when tree view is clicked.
*/
bool GtkTorrentTreeView::torrentView_onClick(GdkEventButton *event)
{
	Gtk::TreeView::on_button_press_event(event);
//	puts("nerds");
//	printf("%d, %d\n", event->type, event->button);
	if(event->type == 5 && event->button == 1) //if double left click
		openView_onClick();

	if(event->button == 3) // if right-click
	{
		m_rcMenu	= Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem1 = Gtk::manage(new Gtk::MenuItem("Start"));
		Gtk::MenuItem *rcmItem2 = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::MenuItem *rcmItem3 = Gtk::manage(new Gtk::MenuItem("Remove"));
		Gtk::SeparatorMenuItem *rcSep1 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem4 = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5 = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		Gtk::SeparatorMenuItem *rcSep2 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem6 = Gtk::manage(new Gtk::MenuItem("Property"));
		rcmItemSeq = Gtk::manage(new Gtk::CheckMenuItem("Sequential Download"));

		rcmItem1->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmItem2->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmItem3->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmItem4->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));

		/* Maybe an onHover or smth for this one. */
		rcmItem5->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmItem6->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));
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

	m_infobar->updateInfo(getFirstSelected());
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

	append_column("#", m_cols.m_col_queue);
	append_column("Age", m_cols.m_col_age);
	append_column("ETA", m_cols.m_col_eta);
	append_column("Name", m_cols.m_col_name);
	append_column("Seed", m_cols.m_col_seeders);
	append_column("Leech", m_cols.m_col_leechers);
	append_column("Up Speed", m_cols.m_col_ul_speed);
	append_column("Down Speed", m_cols.m_col_dl_speed);
	//append_column("Uploaded", m_cols.m_col_ul_total);
	//append_column("Downloaded", m_cols.m_col_dl_total);
	append_column("Size", m_cols.m_col_size);
	append_column("Remains", m_cols.m_col_remaining);
	append_column("Ratio", m_cols.m_col_dl_ratio);

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
	row[m_cols.m_col_name]       = t->getHandle().name();
	row[m_cols.m_col_seeders]    = t->getTotalSeeders();
	row[m_cols.m_col_leechers]   = t->getTotalLeechers();
	//row[m_cols.m_col_ul_total]   = t->getTextTotalUploaded();
	//row[m_cols.m_col_dl_total]   = t->getTextTotalDownloaded();
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
		c[m_cols.m_col_eta]        = t->getTextEta();
		c[m_cols.m_col_percent]    = t->getTotalProgress();
		c[m_cols.m_col_seeders]    = t->getTotalSeeders();
		c[m_cols.m_col_leechers]   = t->getTotalLeechers();
		c[m_cols.m_col_name]       = t->getHandle().name();
		c[m_cols.m_col_ul_speed]   = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed]   = t->getTextDownloadRate();
		c[m_cols.m_col_ul_total]   = t->getTextTotalUploaded();
		c[m_cols.m_col_dl_total]   = t->getTextTotalDownloaded();
		c[m_cols.m_col_size]       = t->getTextSize();
		c[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
		c[m_cols.m_col_eta]        = t->getTextTimeRemaining();
		c[m_cols.m_col_background] =  m_colors[fgbg].first;
		c[m_cols.m_col_foreground] =  m_colors[fgbg].second;

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

	if(t == nullptr)
		return;
	if(!t->getHandle().status().has_metadata)
		return;

	Glib::RefPtr<Gio::AppInfo> fileHandler = Gio::AppInfo::create_from_commandline("xdg-open", "If I knew I wouldn't ask, you turbonerd.", Gio::APP_INFO_CREATE_SUPPORTS_URIS);
	t->getTextState();
	auto files = t->getHandle().get_torrent_info().files();
	string path = Application::getSingleton()->getCore()->getDefaultSavePath() + '/' + t->getHandle().get_torrent_info().file_at(0).path;
	if (files.num_files() > 1) // if there's more than a file, we open the containing folder
		path = path.substr(0, path.find_last_of('/'));
	Glib::RefPtr<Gio::File> thingtoopen = Gio::File::create_for_path(path);
	fileHandler->launch(thingtoopen);
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
		t[i]->getHandle().set_sequential_download(rcmItemSeq->get_active());
}

/**
* Prepares the drawing of the sequential download right click menu item according to the selected torrents.
*/
void GtkTorrentTreeView::sequentialChange_onRealize()
{
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();

	if(selectedIndices().size() > 0)
	{
		bool firstIsSeq = t[selectedIndices()[0]]->getHandle().status().sequential_download;
		for (auto i : selectedIndices())
			if(t[i]->getHandle().status().sequential_download != firstIsSeq)
			{
				rcmItemSeq->set_inconsistent();
				return;
			}
		rcmItemSeq->set_active(firstIsSeq);
	}
	else
		rcmItemSeq->set_active(false);
}
