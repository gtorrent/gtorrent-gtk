#include<gtorrent/Settings.hpp>
#include<gtorrent/Platform.hpp>

#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/separatormenuitem.h>

#include "../Application.hpp"
#include "GtkFileTreeView.hpp"

GtkFileTreeView::GtkFileTreeView()
{
	m_liststore = Gtk::TreeStore::create(m_cols);
	set_model(m_liststore);
	setupColumns();
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkFileTreeView::fileView_onClick), false);
}

bool GtkFileTreeView::fileColumns_onClick(GdkEventButton *event)
{
	return false;
}

bool GtkFileTreeView::fileView_onClick(GdkEventButton *event)
{
	return false;
}
// Seems to work with torrent that are 1 or 2 node deep, but it should require further testing.
void GtkFileTreeView::populateTree(FileTree &ft, Gtk::TreeRow *row)
{
	// TODO: size column shall be in the format "x of y" where x is size of block*downloaded block and y is size of block*number of block in file
	if(ft.children.size() == 0)
	{
		Gtk::TreeRow childr;
		if(row)
			childr = *m_liststore->append(row->children());
		else
			childr = *m_liststore->append();

		// TODO: Fill file attributes here, the priority column shall contain an interger and use a text cell renderer to render a string coresponding to the priority
		// if your just here to fill the rows and don't want to look at the code behind this, assume the following
		// childr = the row, ft.fs = associated file_storage ft.t = torrent pointer
		childr[m_cols.m_col_name] = ft.filename;
		childr[m_cols.m_col_size] = getFileSizeString(ft.fs.at(ft.index).size);
		return;
	}
	Gtk::TreeRow childr = row
		? childr = *m_liststore->append(row->children())
		: childr = *m_liststore->append();
	for(auto i : ft.children)
	{
		// TODO: File folder attributes here, the size column and progress bar are respectivly a recursive sum and recursive average of their children
		// don't use ft.fs here.
		childr[m_cols.m_col_name] = ft.filename;
		populateTree(*i.second, &childr);
	}
}

void GtkFileTreeView::select(std::shared_ptr<gt::Torrent> selected)
{
	m_liststore->clear();
	auto filestorage = selected->getInfo()->files();
	FileTree ft(filestorage, selected);
	for(auto i : selected->filenames())
		ft.add(i);

	if(selected->filenames().size() != 1)
	{
		FileTree *content = ft.children.begin()->second;
		std::cout << "LLEL " << content->filename << std::endl;
		for(auto i : content->children)
		{
			std::cout << i.second->filename << std::endl;
			populateTree(*i.second, nullptr);
		}
	}
	else // if 1 node-deep then there is no root folder
		for(auto i : ft.children)
			populateTree(*i.second, nullptr);

	set_model(m_liststore);
}

void GtkFileTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererProgress *cell = nullptr;

	if(gt::Settings::settings["FileColumnsProperties"] != "")
		loadColumns();
	else
	{
		append_column(      "Name", m_cols.m_col_name);
		append_column(      "Size", m_cols.m_col_size);
		append_column(  "Priority", m_cols.m_col_priority);
	}

	for (auto & c : this->get_columns())
	{
/*		c->add_attribute(dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell())->property_background(), m_cols.m_col_background);
		c->add_attribute(dynamic_cast<Gtk::CellRendererText *>(c->get_first_cell())->property_foreground(), m_cols.m_col_foreground);
*/	}

	cell = Gtk::manage(new Gtk::CellRendererProgress());
	cid = this->append_column("Progress", *cell);
	col = this->get_column(cid - 1);
/*	col->add_attribute(cell->property_value(), 0);//m_cols.m_col_percent);
	col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	col->add_attribute(cell->property_cell_background(), m_cols.m_col_background);
*/
	for (auto &c : this->get_columns())
	{
		Gtk::Button *butt = c->get_button();
		butt->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkFileTreeView::fileColumns_onClick));
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		c->set_alignment(0.5f);
		c->set_clickable();
		c->set_resizable();
		c->set_reorderable();
		if(gt::Settings::settings["FileColumnsProperties"] == "")
			c->set_fixed_width(120);

	}
	if(gt::Settings::settings["ColumnsProperties"] == "")
		get_column(0)->set_fixed_width(48);
}

void GtkFileTreeView::saveColumns()
{
	std::string cStates;
	for(auto &c : get_columns())
		cStates += c->get_title() + '|' + std::to_string(c->get_width()) + '|' + ((c->get_visible()) ? 'v' : 'h') + ',';
	gt::Settings::settings["FileColumnsProperties"] = cStates;
}

// This is where it gets tricky/ugly.
void GtkFileTreeView::loadColumns()
{
	std::vector<std::string> titles = { "Name", "Size", "Progress", "Priority" };
	std::vector<Gtk::TreeModelColumnBase*> cols
	{
		&m_cols.m_col_name,
		&m_cols.m_col_size,
		&m_cols.m_col_priority
	};
	std::string tmp = gt::Settings::settings["FileColumnsProperties"];
	if (tmp == "")
		tmp = "Name|250|v,Size|95|v,Progress|160|v,Priority|160|v,";

	do
	{
		std::string title = tmp.substr(0, tmp.find('|'));
		tmp = tmp.substr(tmp.find('|') + 1);
		int width = stoi(tmp.substr(0, tmp.find('|')));
		tmp = tmp.substr(tmp.find('|') + 1);
		bool hidden = tmp.substr(0, tmp.find(','))[0] == 'h';
		tmp = tmp.substr(tmp.find(',') + 1);
		int index = find(titles.begin(), titles.end(), title) - titles.begin();
		if(index == 4) continue;
		else
		{
			auto k = get_column(append_column(titles[index], *static_cast<Gtk::TreeModelColumn<Glib::ustring>*>(cols[index])) - 1);
			k->set_fixed_width(width);
			k->set_visible(!hidden);
		}
	}
	while (tmp != "");
}
/*
void GtkFileTreeView::addCell(std::shared_ptr<gt::Torrent> &t)
{
	if (t == NULL)
		return;

	Gtk::TreeModel::Row row      = *(m_liststore->append());
	// if there's a % in the state std::string, then the torrent is downloading
	std::string fgbg = t->getTextState().find('%') == std::string::npos ? t->getTextState() : "Downloading";

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
	row[m_cols.m_col_torrent]    =  t;
}
*/


