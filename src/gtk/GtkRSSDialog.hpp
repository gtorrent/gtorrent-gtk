#pragma once

#include <gtorrent/Feed.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>


class GtkMainWindow;

class GtkFeedColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFeedColumns()
		{
			add(name);
			add(feed);
		}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<std::shared_ptr<gt::Feed>> feed;
};


class GtkFunctionColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFunctionColumns()
		{
			add(eval);
		}

	Gtk::TreeModelColumn<Glib::ustring> eval;
};

class GtkRssItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkRssItemColumns()
		{
			add(enabled);
			add(name);
		}

	Gtk::TreeModelColumn<bool> enabled;
	Gtk::TreeModelColumn<std::string> name;
};

class GtkRSSDialog : public Gtk::Dialog
{
	std::shared_ptr<gt::Core> m_core;
	Gtk::Button   *cancelButton = nullptr, *okButton       = nullptr, *addFeedButton  = nullptr, *removeFeedButton = nullptr, *aTogButton  = nullptr, *gToaButton = nullptr, *addFilterBtn = nullptr, *removeFilterBtn = nullptr, *addFunBtn = nullptr, *removeFunBtn = nullptr;
	Gtk::TreeView *rssTreeView  = nullptr, *globalTreeView = nullptr, *activeTreeView = nullptr, *filterTreeView   = nullptr, *funTreeView = nullptr;
	Glib::RefPtr<Gtk::ListStore> rssItemsList, globalFeedsList, activeFeedsList, filtersList, functionsList;

public:
	GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder);
	void setupTreeviews();
	void addNewFeed();
	void removeFeed();
	void moveToActive();
	void removeFromActive();
	void addFilter();
	void removeFilter();
	void addFunction();
	void removeFunction();
	virtual void on_response(int response); // TODO: implement this
};
