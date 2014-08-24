#include <vector>

#include <gtkmm/icontheme.h>
#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/separatormenuitem.h>
#include <giomm/fileinfo.h>
#include <giomm/file.h>

#include <gtorrent/Settings.hpp>
#include <gtorrent/Platform.hpp>
#include <gtorrent/Torrent.hpp>

#include "../Application.hpp"
#include "GtkFileTreeView.hpp"

int _index = 0; // TODO: Indexing is broken in FileTree, this is a temporary measure
std::string prioStr[] =
{
	"Off",
	"Normal",
	"Above Normal",
	"High",
	"Very High",
	"Ultra High",
	"Mega High",
	"Giga High", // my brother died that way
	"Mixed"
};

Glib::RefPtr<Gtk::IconTheme> iconTheme;

GtkFileTreeView::GtkFileTreeView()
{
	m_liststore = Gtk::TreeStore::create(m_cols);
	get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	iconTheme = Gtk::IconTheme::get_default();
	set_model(m_liststore);
	setupColumns();
	set_enable_tree_lines();
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkFileTreeView::fileView_onClick), false);
}

bool GtkFileTreeView::fileColumns_onClick(GdkEventButton *event)
{
	return false;
}

bool GtkFileTreeView::fileView_onClick(GdkEventButton *event)
{
	if(event->type == 5 && event->button == 1) //if double left click
		openView_onClick();

	if(event->button == 3) // if right-click
	{
		if(selectedRows().size() == 0) return false;

		m_rcMenu                       = Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem2        = Gtk::manage(new Gtk::MenuItem((selectedRows()[0][m_cols.m_col_prioritylevel] == 0) ? "Start" : "Stop"));
		Gtk::MenuItem *rcmItem3        = Gtk::manage(new Gtk::MenuItem("Rename"));
		Gtk::SeparatorMenuItem *rcSep1 = Gtk::manage(new Gtk::SeparatorMenuItem());
		Gtk::MenuItem *rcmItem4        = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5        = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there

		Gtk::Menu *submenu = Gtk::manage(new Gtk::Menu());

		for(int i = 0; i < 8; ++i)
		{
			Gtk::MenuItem *smItem = Gtk::manage(new Gtk::MenuItem(prioStr[i]));
			smItem->signal_activate().connect(sigc::bind<1>(sigc::mem_fun(*this, &GtkFileTreeView::setSelectedPriorities), i));
			submenu->add(*smItem);
		}
		rcmItem5->set_submenu(*submenu);

		rcmItem2->signal_activate ().connect(sigc::mem_fun(*this, &GtkFileTreeView::toggleView_onClick));
//		rcmItem3->signal_activate ().connect(sigc::mem_fun(*this, &GtkFileTreeView::rename_onClick)); // not really sure who the fuck rename manually their files, btw, also may be hard to actually implement
		rcmItem4->signal_activate ().connect(sigc::mem_fun(*this, &GtkFileTreeView::openView_onClick));

		m_rcMenu->add(*rcmItem2);
		m_rcMenu->add(*rcmItem3);
		m_rcMenu->add(*rcSep1);
		m_rcMenu->add(*rcmItem4);
		m_rcMenu->add(*rcmItem5);
		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}

	if(is_blank_at_pos(event->x, event->y) && event->send_event == false) // is_blank_at_pos return true even if the blank is the background of a row... so I just resend the click.
	{
		get_selection()->unselect_all();
		event->send_event = true; // why doesn't put() do that
		Gdk::Event((GdkEvent*)event).put();
	}

	return false;
}

/**
 * Gets collective information from all the children recursively
 * This is for the initial set up only.
 * @param size Cumulative file size of all children (in bytes?)
 * @param progress Average progress of all children
 * @param total Number of children
 */
void GtkFileTreeView::getChildAttributes(FileTree &ft, long &size, int &state, double &progress, int &priority, int &total)
{
	/* Reached the end of recursion call */
	if(ft.children.size() == 0)
	{
		size += ft.fs.at(ft.index).size;
		priority = (priority == -1 ? ft.t->getHandle().file_priority(ft.index) : (priority != ft.t->getHandle().file_priority(ft.index) ? 8 : priority));
		state = priority == 8 ? 2 : priority != 0;
		++total;

		progress += double(progress_all[ft.index]) / ft.fs.file_size(ft.index);
		return;
	}

	/* For each child */
	for(auto i : ft.children)
	{
		getChildAttributes(*i.second, size, state, progress, priority, total);
	}

	/* Complete all the calculations */
	progress /= total;
}

/**
 * Gets collective information from all the children recursively
 * This overload is for once the gtk tree widget has been built.
 * @param size Cumulative file size of all children (in bytes?)
 * @param progress Average progress of all children
 * @param total Number of children
 */
void GtkFileTreeView::getChildAttributes(Gtk::TreeRow &row, long &size, int &state, double &progress, int &priority, int &deepness)
{
	if(row.children().size() == 0)
	{
		libtorrent::file_entry entry = row[m_cols.m_col_entry];
		size += entry.size;
		priority = row[m_cols.m_col_prioritylevel];
		state = priority == 8 ? 2 : priority != 0;
		++deepness;
		progress += double(progress_all[row[m_cols.m_col_index]]) / torrent->getInfo()->files().file_size(row[m_cols.m_col_index]);
		return;
	}
	for(auto child : row.children())
	{
		getChildAttributes(child, size, state, progress, priority, deepness);
	}
	progress /= deepness;

}

// Seems to work with torrent that are 1 or 2 node deep, but it should require further testing.
void GtkFileTreeView::populateTree(FileTree &ft, Gtk::TreeRow *row)
{
/*
        std::vector<libtorrent::size_type> progress_all;
        ft.t->getHandle().file_progress(progress_all, 1);

        std::cout << "Length: " << progress_all.size() << std::endl;

>>>>>>> wip/looks-familiar*/
	// TODO: size column shall be in the format "x of y" where x is size of block*downloaded block and y is size of block*number of block in file
	/* If the FileTree doesn't contain any children */
	if(ft.children.size() == 0)
	{
		Gtk::TreeRow childr;
		Gtk::IconInfo iconInfo;

		childr = row
			? *m_liststore->append(row->children())
			: *m_liststore->append();

		childr[m_cols.m_col_fullpath] = Glib::build_filename(ft.t->getSavePath() + ft.parent->fullname(), ft.filename);

		if(torrent->getInfo()->files().is_valid() && gt::Platform::checkDirExist(ft.t->getSavePath() + '/' + ft.fullname()))
			iconInfo = iconTheme->lookup_icon(Gio::File::create_for_path(childr[m_cols.m_col_fullpath])->query_info()->get_icon(), 16, Gtk::ICON_LOOKUP_USE_BUILTIN);
		else
			iconInfo = iconTheme->lookup_icon("gtk-file", 16, Gtk::ICON_LOOKUP_USE_BUILTIN);

		childr[m_cols.m_col_index]         = ft.index;
		childr[m_cols.m_col_name]          = ft.filename;
		childr[m_cols.m_col_entry]         = ft.fs.at(ft.index);
		childr[m_cols.m_col_size]          = getFileSizeString(ft.fs.at(ft.index).size); // TODO: For some reason the reported size here is wrong, to fix, urgent.
		childr[m_cols.m_col_icon]          = iconInfo.load_icon();
		childr[m_cols.m_col_percent]       = int(progress_all[ft.index] * 100 / ft.fs.file_size(ft.index));
		childr[m_cols.m_col_priority]      = prioStr[ft.t->getHandle().file_priority(ft.index)];
		childr[m_cols.m_col_prioritylevel] = ft.t->getHandle().file_priority(ft.index);
		childr[m_cols.m_col_percent_text]  = std::to_string(childr[m_cols.m_col_percent]) + '%';
		childr[m_cols.m_col_activated]     = ft.t->getHandle().file_priority(ft.index) != 0;

		return;
	}

	Gtk::TreeRow childr = row
		? childr = *m_liststore->append(row->children())
		: childr = *m_liststore->append();

	/* If the FileTree contains children */
	for(auto i : ft.children)
	{
		// TODO: File folder attributes here, the size column and progress bar are respectivly a recursive sum and recursive average of their children
		// don't use ft.fs here.
		long totalSize = 0;
		int state = 0, priority = -1, deepness = 0;
		double progress = 0;
		getChildAttributes(ft, totalSize, state, progress, priority, deepness); // state = 0 = off, 1 = enabled, 2 = inconsistent, priority = 0-7 as expected and 8 for mixed

		childr[m_cols.m_col_name]           = ft.filename;
		childr[m_cols.m_col_size]           = getFileSizeString(totalSize);
		childr[m_cols.m_col_priority]       = prioStr[priority];
		childr[m_cols.m_col_prioritylevel]  = priority;
		childr[m_cols.m_col_percent]        = int(progress * 100);
		childr[m_cols.m_col_percent_text]   = std::to_string(childr[m_cols.m_col_percent]) + '%';
		childr[m_cols.m_col_activated]      = state == 1;
		childr[m_cols.m_col_inconsistent]   = state == 2;
		Gtk::IconInfo iconInfo              = iconTheme->lookup_icon("folder", 16, Gtk::ICON_LOOKUP_USE_BUILTIN);
		childr[m_cols.m_col_icon]           = iconInfo.load_icon();
		childr[m_cols.m_col_index]          = ft.index;

		populateTree(*i.second, &childr);
	}
}

void GtkFileTreeView::select(std::shared_ptr<gt::Torrent> selected)
{
	if(torrent != selected)
	{
		m_liststore->clear();
		torrent = selected;
		torrent->getHandle().file_progress(progress_all, 1);

		libtorrent::file_storage filestorage = torrent->getInfo()->files();
		FileTree ft(filestorage, torrent);
		for(int i = 0; i < filestorage.num_files(); ++i)
		{
			std::string path = filestorage.at(i).path;
			ft.add(path, i);
		}

		if(torrent->filenames().size() != 1)
			for(auto i : ft.children.begin()->second->children)
				populateTree(*i.second, nullptr);
		else // if 1 node-deep then there is no root folder
			for(auto i : ft.children)
				populateTree(*i.second, nullptr);
		_index = 0;
		set_model(m_liststore);
	}
}

void GtkFileTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;

	Gtk::CellRendererProgress *cell = nullptr;
	Gtk::CellRendererPixbuf  *cellp = nullptr;
	Gtk::CellRendererText    *cellt = nullptr;
	Gtk::CellRendererToggle *cellto = nullptr;

	if(gt::Settings::settings["FileColumnsProperties"] != "")
		loadColumns();
	else
	{
		cellp  = Gtk::manage(new Gtk::CellRendererPixbuf());
		cellt  = Gtk::manage(new Gtk::CellRendererText());
		cellto = Gtk::manage(new Gtk::CellRendererToggle());

		cid = append_column(*Gtk::manage(new Gtk::TreeViewColumn("")));
		col = get_column(cid - 1);

		col->pack_start(*cellto, false);
		col->set_min_width(29);
		col->set_max_width(29);
		col->add_attribute(cellto->property_active(), m_cols.m_col_activated);
		col->add_attribute(cellto->property_inconsistent(), m_cols.m_col_inconsistent);

		cid = append_column(*Gtk::manage(new Gtk::TreeViewColumn("Name")));
		col = get_column(cid - 1);
		set_expander_column(*col);
		col->pack_start(*cellp, false);
		col->pack_start(*cellt, false);
		col->add_attribute(cellt->property_text(), m_cols.m_col_name);
		col->add_attribute(cellp->property_pixbuf(), m_cols.m_col_icon);

		cellto->property_xalign() = 0.5;
		cellto->property_width() = 16;
		cellto->signal_toggled().connect(sigc::mem_fun(*this, &GtkFileTreeView::onCheckBoxClicked), false);
		cellp->property_pixbuf_expander_open() = iconTheme->lookup_icon("folder-open", 16, Gtk::ICON_LOOKUP_USE_BUILTIN).load_icon();
		append_column("Size", m_cols.m_col_size);
		append_column("Priority", m_cols.m_col_priority);
	}

	cell = Gtk::manage(new Gtk::CellRendererProgress());
	cid = this->append_column("Progress", *cell);
	col = this->get_column(cid - 1);
	col->add_attribute(cell->property_value(), m_cols.m_col_percent);
	col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);

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

void GtkFileTreeView::update()
{
	torrent->getHandle().file_progress(progress_all, 1);
	for(auto child : m_liststore->children())
		update(child);
}

void GtkFileTreeView::update(Gtk::TreeRow &row)
{
	if(row.children().size() == 0)
	{
		Gtk::IconInfo iconInfo;
		if(torrent->getInfo()->files().is_valid() && gt::Platform::checkDirExist(row[m_cols.m_col_fullpath]))
			iconInfo = iconTheme->lookup_icon(Gio::File::create_for_path(row[m_cols.m_col_fullpath])->query_info()->get_icon(), 16, Gtk::ICON_LOOKUP_USE_BUILTIN);
		else
			iconInfo = iconTheme->lookup_icon("gtk-file", 16, Gtk::ICON_LOOKUP_USE_BUILTIN);

		row[m_cols.m_col_priority] = prioStr[torrent->getHandle().file_priority(row[m_cols.m_col_index])];
		row[m_cols.m_col_activated] = torrent->getHandle().file_priority(row[m_cols.m_col_index]) != 0;
		row[m_cols.m_col_percent] = int(progress_all[row[m_cols.m_col_index]] * 100 / torrent->getInfo()->files().file_size(row[m_cols.m_col_index]));
		row[m_cols.m_col_percent_text] = std::to_string(row[m_cols.m_col_percent]) + '%';
		row[m_cols.m_col_icon] = iconInfo.load_icon();

		return;
	}
	for(auto i : row.children())
	{
		long totalSize = 0;
		int state = 0, priority = -1, deepness = 0;
		double progress = 0;
		getChildAttributes(row, totalSize, state, progress, priority, deepness); // state = 0 = off, 1 = enabled, 2 = inconsistent, priority = 0-7 as expected and 8 for mixed

		row[m_cols.m_col_size] = getFileSizeString(totalSize);
		row[m_cols.m_col_priority] = prioStr[priority];
		row[m_cols.m_col_percent] = int(progress * 100);
		row[m_cols.m_col_percent_text] = std::to_string(row[m_cols.m_col_percent]) + '%';
		row[m_cols.m_col_activated] = state == 1;
		row[m_cols.m_col_inconsistent] = state == 2;
		update(i);
	}

}

void GtkFileTreeView::setPriority(Gtk::TreeRow &node, int level)
{
	if(node.children().size() == 0)
	{
		torrent->getHandle().file_priority(node[m_cols.m_col_index], level);
		node[m_cols.m_col_activated] = false;
		node[m_cols.m_col_priority] = prioStr[level];
		node[m_cols.m_col_prioritylevel] = level;
		std::cout << node[m_cols.m_col_index] << std::endl;
		return;
	}
	for(auto child : node.children())
		setPriority(child, level);
}

void GtkFileTreeView::onCheckBoxClicked(std::string path)
{
	auto row = *m_liststore->get_iter(Gtk::TreePath(path));
	row[m_cols.m_col_activated] = !row[m_cols.m_col_activated];
	setPriority(row, row[m_cols.m_col_activated]);
	update(row);
}

std::vector<Gtk::TreeRow> GtkFileTreeView::selectedRows()
{
	Glib::RefPtr<Gtk::TreeSelection> sel = get_selection();
	std::vector<Gtk::TreeModel::Path> paths = sel->get_selected_rows();
	std::vector<Gtk::TreeRow> rows;

	for(auto path : paths)
		rows.push_back(*m_liststore->get_iter(path));

	return rows;
}

void GtkFileTreeView::setSelectedPriorities(int level)
{
	auto rows = selectedRows();
	for(auto row : rows)
		setPriority(row, level);
}

void GtkFileTreeView::openView_onClick()
{
	auto row = selectedRows()[0];
	if(torrent == nullptr || !torrent->hasMetadata())
		return;

	// Have to do that because I assumed the index col of a folder contained the index of one of its children
	if(row.children().size() != 0)
		gt::Platform::openTorrent(torrent, row.children()[0][m_cols.m_col_index], row.children().size() != 0);
	else
		gt::Platform::openTorrent(torrent, row[m_cols.m_col_index]);

}

void GtkFileTreeView::toggleView_onClick()
{
	setSelectedPriorities(selectedRows()[0][m_cols.m_col_prioritylevel] == 0);
}
