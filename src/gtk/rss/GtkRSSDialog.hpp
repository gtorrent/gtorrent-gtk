#pragma once

#include <gtorrent/Feed.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/checkbutton.h>


class GtkMainWindow;

class GtkFeedColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFeedColumns()
		{
			add(name);
			add(feed);
		}

	Gtk::TreeModelColumn<std::string> name;
	Gtk::TreeModelColumn<std::shared_ptr<gt::Feed>> feed;
};

class GtkFunctionColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFunctionColumns()
		{
			add(eval);
		}

	Gtk::TreeModelColumn<std::string> eval;
};

class GtkFilterColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkFilterColumns()
		{
			add(name);
			add(regex);
		}

	Gtk::TreeModelColumn<std::string> name;
	Gtk::TreeModelColumn<std::string> regex;
};

class GtkRssItemColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkRssItemColumns()
		{
			add(enabled);
			add(name);
			add(item);
		}

	Gtk::TreeModelColumn<bool> enabled;
	Gtk::TreeModelColumn<std::string> name;
	Gtk::TreeModelColumn<libtorrent::feed_item> item;
};

class GtkRSSDialog : public Gtk::Dialog
{
	GtkFeedColumns     global, active;
	GtkRssItemColumns  items;
	GtkFilterColumns   filters;
	GtkFunctionColumns functions;

	std::shared_ptr<gt::Core> m_core;
	Gtk::Button   *cancelButton = nullptr, *okButton       = nullptr, *addFeedButton  = nullptr,
		*removeFeedButton = nullptr, *aTogButton  = nullptr, *gToaButton = nullptr,
		*addFilterBtn = nullptr, *removeFilterBtn = nullptr, *addFunBtn = nullptr,
		*removeFunBtn = nullptr;
	Gtk::TreeView *rssTreeView  = nullptr, *globalTreeView = nullptr, *activeTreeView = nullptr,
	*filterTreeView   = nullptr, *funTreeView = nullptr;
	Gtk::CheckButton *rssAuto   = nullptr;
	Glib::RefPtr<Gtk::ListStore> rssItemsList, globalFeedsList, activeFeedsList, filtersList, functionsList;

public:
	std::shared_ptr<gt::FeedGroup> feedg;
	GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder);
	int run(std::string = std::string());
	void setupTreeviews();
	void feedAdd();
	void feedRemove();
	void moveToActive();
	void removeFromActive();
	void filterAdd();
	void filterRemove();
	void functionAdd();
	void functionRemove();
	void toggleAutoAdd();
	virtual void on_response(int response); // TODO: implement this
};
