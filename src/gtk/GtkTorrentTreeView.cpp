#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/treeviewcolumn.h>
#include <Application.hpp>
#include "GtkTorrentTreeView.hpp"

GtkTorrentTreeView::GtkTorrentTreeView()
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
}

bool GtkTorrentTreeView::torrentView_onClick(GdkEventButton *event)
{
	if(event->button == 3)
	{
		m_rcMenu	= Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem1 = Gtk::manage(new Gtk::MenuItem("Start"));
		Gtk::MenuItem *rcmItem2 = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::MenuItem *rcmItem3 = Gtk::manage(new Gtk::MenuItem("Remove"));
		/* If someone finds out how to put a horizontal separator in the menu like on web browsers  put it here */
		Gtk::MenuItem *rcmItem4 = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5 = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		/* If someone finds out how to put a horizontal separator in the menu like on web browsers  put it here */
		Gtk::MenuItem *rcmItem6 = Gtk::manage(new Gtk::MenuItem("Property"));

		rcmItem1->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmItem2->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmItem3->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmItem4->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));

		/* Maybe an onHover or smth for this one. */
		rcmItem5->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmItem6->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));

		m_rcMenu->add(*rcmItem1);
		m_rcMenu->add(*rcmItem2);
		m_rcMenu->add(*rcmItem3);
		m_rcMenu->add(*rcmItem4);
		m_rcMenu->add(*rcmItem5);
		m_rcMenu->add(*rcmItem6);

		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}

	return false;
}

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

// TODO REFACTOR THE LIVING HELL OUT OF THIS ABOMINATION -- nyanpasu
void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererProgress *cell = nullptr;

	append_column("Queue", m_cols.m_col_queue);
	append_column("Age", m_cols.m_col_age);
	append_column("ETA", m_cols.m_col_eta);
	append_column("Name", m_cols.m_col_name);
	append_column("Seed", m_cols.m_col_seeders);
	append_column("Leech", m_cols.m_col_leechers);
	append_column("Upload Speed", m_cols.m_col_ul_speed);
	append_column("Download Speed", m_cols.m_col_dl_speed);
	append_column("Uploaded", m_cols.m_col_ul_total);
	append_column("Downloaded", m_cols.m_col_dl_total);
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
		c->set_fixed_width(96);
	}
}

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
	row[m_cols.m_col_ul_total]   = t->getTextTotalUploaded();
	row[m_cols.m_col_dl_total]   = t->getTextTotalDownloaded();
	row[m_cols.m_col_size]       = t->getTextSize();
	row[m_cols.m_col_remaining]  = t->getTextRemaining();
	row[m_cols.m_col_dl_ratio]   = t->getTextTotalRatio();
	row[m_cols.m_col_background] =  m_colors[fgbg].first;
	row[m_cols.m_col_foreground] =  m_colors[fgbg].second;
}

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

shared_ptr<gt::Torrent> GtkTorrentTreeView::getFirstSelected()
{
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();
	if(selectedIndices().size() < 1)
		return nullptr;
	else
		return t[selectedIndices()[0]];
}

void GtkTorrentTreeView::setSelectedPaused(bool isPaused)
{
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();
	for (auto i : selectedIndices())
		t[i]->setPaused(isPaused);// the pause button switches the status

}

void GtkTorrentTreeView::stopView_onClick()
{
	/* Doesn't do nuffin wrong */
}
void GtkTorrentTreeView::openView_onClick()
{
	/* Doesn't do nuffin wrong */
}
void GtkTorrentTreeView::startView_onClick()
{
	/* Doesn't do nuffin wrong */
}
void GtkTorrentTreeView::removeView_onClick()
{
	/* Doesn't do nuffin wrong */
}
void GtkTorrentTreeView::priorityView_onClick()
{
	/* Doesn't do nuffin wrong */
}
void GtkTorrentTreeView::propertyView_onClick()
{
	/* Doesn't do nuffin wrong */
}