#include "GtkRSSDialog.hpp"
#include "GtkGenericAddDialog.hpp"
#include "../Application.hpp"

// TODO: Rename "Functions" to "Criterions" or equivalent ?

GtkRSSDialog::GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	m_core = Application::getSingleton()->getCore();

	rbuilder->get_widget(    "cancelButton",     cancelButton);
	rbuilder->get_widget(        "okButton",         okButton);
	rbuilder->get_widget(   "addFeedButton",    addFeedButton);
	rbuilder->get_widget("removeFeedButton", removeFeedButton);
	rbuilder->get_widget(      "aTogButton",       aTogButton);
	rbuilder->get_widget(      "gToaButton",       gToaButton);
	rbuilder->get_widget(    "addFilterBtn",     addFilterBtn);
	rbuilder->get_widget( "removeFilterBtn",  removeFilterBtn);
	rbuilder->get_widget(    "addFunButton",        addFunBtn);
	rbuilder->get_widget( "removeFunButton",     removeFunBtn);
	rbuilder->get_widget("functionTreeView",      funTreeView);
	rbuilder->get_widget(  "filterTreeView",   filterTreeView);
	rbuilder->get_widget(  "activeTreeView",   activeTreeView);
	rbuilder->get_widget(  "globalTreeView",   globalTreeView);
	rbuilder->get_widget(     "rssTreeView",      rssTreeView);

	addFeedButton   ->signal_clicked().connect([this](){addNewFeed      ();});
	removeFeedButton->signal_clicked().connect([this](){removeFeed      ();});
	aTogButton      ->signal_clicked().connect([this](){moveToActive    ();});
	gToaButton      ->signal_clicked().connect([this](){removeFromActive();});
	addFilterBtn    ->signal_clicked().connect([this](){addFilter       ();});
	removeFilterBtn ->signal_clicked().connect([this](){removeFilter    ();});
	addFunBtn       ->signal_clicked().connect([this](){addFunction     ();});
	removeFunBtn    ->signal_clicked().connect([this](){removeFunction  ();});


	//Need to rewrite a part of wip/rss in core to continue
/*
	funTreeView
	filterTreeView
	activeTreeView
	globalTreeView
	rssTreeView
*/
}

void GtkRSSDialog::on_response(int response)
{
	// When the feeds will be create (in another method, to let the user test their filter/settings) the callback methos will be overriden
	// to fill the main treeview, on_response will be called when the user either click cancel or confirm, so here, we will just reset to callback to their defaults
	// (either auto add or run a notification when a new item is available, for example) or if "cancel", trash the group.
}

void GtkRSSDialog::addNewFeed()
{
	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/genericdialog.ui");
	GtkGenericAddDialog *addFeedDialog = nullptr;
	refBuilder->get_widget_derived("genericDialog", addFeedDialog);
	addFeedDialog->set_transient_for(*this);
	addFeedDialog->set_default_response(1);
	addFeedDialog->set_title("Add Feed");
	addFeedDialog->label1->set_text("Feed Name:");
	addFeedDialog->label2->set_text("Feed URL:");
	if(addFeedDialog->run() == 1)
	{
		// create and update feed and add it to the global feeds
		abort();
	}
}

// there's no constraint in removing a feed from the active list nor the global list since filters don't depend on them
void GtkRSSDialog::removeFeed()
{
	// a feed can be removed only from the global list, and will be removed from the active list of every other rss group
	// unless the feed isn't used in any other group, the user will be warned that removing this feeds will impact other rss groups
}

void GtkRSSDialog::moveToActive()
{
	// when a feed is moved from the global list to the active list for a group, it is made available
	// in the global list of other groups
}

void GtkRSSDialog::removeFromActive()
{
	// Removes a feed from the group.
	// a feed is available in the global list only if it is 
	// used in another group, or has been added during the session
	// (so unused feeds (those that are in the global list but not used in any group) aren't preserved between sessions)
}

void GtkRSSDialog::addFilter()
{
	// filters are run on every items from every active feed
	// Filter have uniques names, but can have same regexes
	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/genericdialog.ui");
	GtkGenericAddDialog *addFeedDialog = nullptr;
	refBuilder->get_widget_derived("genericDialog", addFeedDialog);
	addFeedDialog->set_transient_for(*this);
	addFeedDialog->set_title("Add Filter");
	addFeedDialog->label1->set_text("Filter Name:");
	addFeedDialog->label2->set_text("Filter Regex:");
	if(addFeedDialog->run() == 1)
	{
		abort();
	}	
}

void GtkRSSDialog::removeFilter()
{
	// You can remove a filter only if it's  not used by any function
	// if the user tries to remove an used filter, he should be prompted 
	// if he wants the functions removed or cancel the operation
}

void GtkRSSDialog::addFunction()
{
	// you can add a function only if the the filter specified in the lhs exists
	// if the specified filter doesn't exist, the user should warned, and if he
	// wants to proceed, a "New Filter" dialog will be shown before adding the function
}

void GtkRSSDialog::removeFunction()
{
	// there's no constraint in removing a function
}
