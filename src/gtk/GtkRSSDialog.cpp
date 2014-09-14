#include "GtkRSSDialog.hpp"
#include "GtkGenericAddDialog.hpp"
#include "GtkFunctionDialog.hpp"
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
	set_default_response(1);

	rssItemsList    = Gtk::ListStore::create(    items);
	globalFeedsList = Gtk::ListStore::create(   global);
	activeFeedsList = Gtk::ListStore::create(   active);
	filtersList     = Gtk::ListStore::create(  filters);
	functionsList   = Gtk::ListStore::create(functions);

	rssTreeView   ->set_model(   rssItemsList);
	funTreeView   ->set_model(  functionsList);
	filterTreeView->set_model(    filtersList);
	globalTreeView->set_model(globalFeedsList);
	activeTreeView->set_model(activeFeedsList);

	addFeedButton   ->signal_clicked().connect([this](){addNewFeed      ();});
	removeFeedButton->signal_clicked().connect([this](){removeFeed      ();});
	aTogButton      ->signal_clicked().connect([this](){removeFromActive();});
	gToaButton      ->signal_clicked().connect([this](){moveToActive    ();});
	addFilterBtn    ->signal_clicked().connect([this](){addFilter       ();});
	removeFilterBtn ->signal_clicked().connect([this](){removeFilter    ();});
	addFunBtn       ->signal_clicked().connect([this](){addFunction     ();});
	removeFunBtn    ->signal_clicked().connect([this](){removeFunction  ();});

	funTreeView   ->append_column("", functions.eval);
	filterTreeView->append_column("", filters.name);
	activeTreeView->append_column("",  active.name);
	globalTreeView->append_column("",  global.name);
	rssTreeView   ->append_column("",   items.name);
}

int GtkRSSDialog::run(std::string fName)
{
	// If run is called with a valid parameter, then we edit the passed feed, else we create a new
	feedg = m_core->addFeedGroup(fName); // returns a pointer to existing one if one with same name exists

	set_title(fName);

	filtersList    ->clear();
	rssItemsList   ->clear();
	functionsList  ->clear();
	globalFeedsList->clear();
	activeFeedsList->clear();

	rssTreeView   ->set_model(   rssItemsList);
	funTreeView   ->set_model(  functionsList);
	filterTreeView->set_model(    filtersList);
	globalTreeView->set_model(globalFeedsList);
	activeTreeView->set_model(activeFeedsList);

	rssTreeView   ->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	funTreeView   ->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	filterTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	globalTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	activeTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);

	for(auto f : m_core->m_feedhandles)
	{
		if(std::find(feedg->m_feeds.begin(), feedg->m_feeds.end(), f) != feedg->m_feeds.end()) continue; //if the feed is used in the group, then it'll be added to the active view

		Gtk::TreeRow row = *globalFeedsList->append();
		if(f->get_feed_status().title != "")
			row[global.name] = f->get_feed_status().title;
		else
			row[global.name] = f->get_feed_status().url;
		row[global.feed] = f;
	}

	for(auto f : feedg->m_feeds)
	{
		Gtk::TreeRow row = *activeFeedsList->append();
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
		for(auto item : f->get_feed_status().items)
		{
			Gtk::TreeRow row = *rssItemsList->append();
			row[items.name] = item.title;
			row[items.item] = item;
		}
	}

	for(auto f : feedg->filters)
	{
		Gtk::TreeRow row = *filtersList->append();
		row[filters.name]  = f.first;
		row[filters.regex] = f.second;
	}

	for(auto f : feedg->functions)
	{
		Gtk::TreeRow row = *functionsList->append();
		row[functions.eval]  = f;
	}

	return Gtk::Dialog::run();
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
	GtkGenericAddDialog *addFilterDialog = nullptr;
	refBuilder->get_widget_derived("genericDialog", addFilterDialog);
	addFilterDialog->set_transient_for(*this);
	addFilterDialog->set_default_response(1);
	addFilterDialog->set_title("Add Feed");
	addFilterDialog->label1->set_text("Feed Name:");
	addFilterDialog->label2->set_text("Feed URL:");

	if(addFilterDialog->run() == 1)
	{
		if(addFilterDialog->entry1->get_text() != "" && addFilterDialog->entry2->get_text() != "")
		{
			Gtk::TreeRow row = *globalFeedsList->append();
			row[global.name] = addFilterDialog->entry1->get_text();
			auto f = m_core->addFeed(addFilterDialog->entry2->get_text());
			row[global.feed] = f;
			f->owners.insert(feedg);
		}
	}
	delete addFilterDialog;
}

// there's no constraint in removing a feed from the active list nor the global list since filters don't depend on them
void GtkRSSDialog::removeFeed()
{
	// a feed can be removed only from the global list, and will be removed from the active list of every other rss group
	// unless the feed isn't used in any other group, the user will be warned that removing this feeds will impact other rss groups
	
	bool doAnyway = false;
	// when a feed is moved from the global list to the active list for a group, it is made available
	// in the global list of other groups
	auto sel   = globalTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(globalTreeView->get_model(), path));
	for (auto i : rows)
	{
		std::shared_ptr<gt::Feed> f = (*globalFeedsList->get_iter(i.get_path()))[global.feed];
		if(!doAnyway && !f->owners.empty())
		{
			auto errorDial = std::make_shared<Gtk::MessageDialog>("This feed is used in other groups!\nRemoving it will also remove it from every other groups.\nProceed ?",
																  false,
																  Gtk::MESSAGE_QUESTION,
																  Gtk::BUTTONS_YES_NO);
			errorDial->set_default_response(Gtk::RESPONSE_NO);
			switch(errorDial->run())
			{
			case Gtk::RESPONSE_YES: doAnyway = true; break;
			default: return;
			}
		}
		for(auto group : f->owners)
			group->m_feeds.erase(f);
		f->owners.clear();
		globalFeedsList->erase(globalTreeView->get_model()->get_iter(i.get_path()));
	}
}

void GtkRSSDialog::moveToActive()
{
	std::vector<std::shared_ptr<gt::Feed>> moving;
	// when a feed is moved from the global list to the active list for a group, it is made available
	// in the global list of other groups
	auto sel   = globalTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(globalTreeView->get_model(), path));
	for (auto i : rows)
	{
		moving.push_back((*globalFeedsList->get_iter(i.get_path()))[global.feed]);
		globalFeedsList->erase(globalTreeView->get_model()->get_iter(i.get_path()));
	}
	for(auto f : moving)
	{
		Gtk::TreeRow row = *activeFeedsList->append();
		feedg->m_feeds.insert(f);
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
		for(auto item : f->get_feed_status().items)
		{
			Gtk::TreeRow row = *rssItemsList->append();
			row[items.name] = item.title;
			row[items.item] = item;
		}
	}
}

void GtkRSSDialog::removeFromActive()
{
	// Removes a feed from the group.
	// a feed is available in the global list only if it is 
	// used in another group, or has been added during the session
	// (so unused feeds (those that are in the global list but not used in any group) aren't preserved between sessions)

	std::vector<std::shared_ptr<gt::Feed>> moving;
	auto sel   = activeTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(activeFeedsList, path));
	for (auto i : rows)
	{
		std::shared_ptr<gt::Feed> f = (*activeFeedsList->get_iter(i.get_path()))[active.feed];
		moving.push_back(f);
		for(auto item : f->get_feed_status().items)
		{
			for(auto row : rssItemsList->children())
				if(item.url == libtorrent::feed_item(row[items.item]).url)
				{
					rssItemsList->erase(row);
					break;
				}
		}
		activeFeedsList->erase(activeFeedsList->get_iter(i.get_path()));
	}

	for(auto f : moving)
	{
		Gtk::TreeRow row = *globalFeedsList->append();
		f->owners.erase(feedg);
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
	}
}

void GtkRSSDialog::addFilter()
{
	// filters are run on every items from every active feed
	// Filter have uniques names, but can have same regexes
	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/genericdialog.ui");
	GtkGenericAddDialog *addFilterDialog = nullptr;
	refBuilder->get_widget_derived("genericDialog", addFilterDialog);
	addFilterDialog->set_transient_for(*this);
	addFilterDialog->set_default_response(1);
	addFilterDialog->set_title("Add Filter");
	addFilterDialog->label1->set_text("Filter Name:");
	addFilterDialog->label2->set_text("Filter Regex:");
	if(addFilterDialog->run() == 1)
	{
		if(addFilterDialog->entry1->get_text() != "" && addFilterDialog->entry2->get_text() != "")
		{
			Gtk::TreeRow row = *filtersList->append();
			feedg->filters[addFilterDialog->entry1->get_text()] = addFilterDialog->entry2->get_text();
			row[filters.name] = addFilterDialog->entry1->get_text();
			row[filters.regex] = addFilterDialog->entry2->get_text();
		}
	}
	delete addFilterDialog;
}

void GtkRSSDialog::removeFilter()
{
	// You can remove a filter only if it's  not used by any function
	// if the user tries to remove an used filter, he should be prompted 
	// if he wants the functions removed or cancel the operation
	std::vector<std::shared_ptr<gt::Feed>> removing;
	auto sel   = filterTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(filtersList, path));

	for (auto i : rows)
	{
		Gtk::TreeRow row = *filtersList->get_iter(i.get_path());
		std::string name = row[filters.name];
		for(auto fun : feedg->functions)
			if(fun.find(name) != std::string::npos)
				return; // if filter is used in function, dont delete it // TODO: show an error dialog here
		feedg->filters.erase(name);
		filtersList->erase(filtersList->get_iter(i.get_path()));
	}
}

void GtkRSSDialog::addFunction()
{
	// you can add a function only if the the filter specified in the lhs exists
	// if the specified filter doesn't exist, the user should warned, and if he
	// wants to proceed, a "New Filter" dialog will be shown before adding the function
	if(feedg->filters.empty())
	{
		auto errorDial = std::make_shared<Gtk::MessageDialog>("You need to add filters in order to add functions !");
		errorDial->run();
		return;
	}

	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/functiondialog.ui");
	GtkFunctionDialog *addFunctionDialog = nullptr;
	refBuilder->get_widget_derived("functiondialog", addFunctionDialog);
	addFunctionDialog->set_transient_for(*this);
	addFunctionDialog->set_default_response(1);
	addFunctionDialog->set_title("Function");
	addFunctionDialog->setFeedGroup(feedg);

	if(addFunctionDialog->run() == 1)
	{
		Gtk::TreeRow row = *functionsList->append();
		std::string fun = addFunctionDialog->filter->get_active_text();
		int op = addFunctionDialog->filter->get_active_row_number();
		std::string ops = "<>=!";
		fun += " " + ops[op] + ' ';
		fun += addFunctionDialog->constant->get_text();
		feedg->functions.insert(fun);
		row[functions.eval] = fun;
	}
	delete addFunctionDialog;
}

void GtkRSSDialog::removeFunction()
{
	std::vector<std::shared_ptr<gt::Feed>> removing;
	auto sel   = funTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(functionsList, path));

	for (auto i : rows)
	{
		Gtk::TreeRow row = *functionsList->get_iter(i.get_path());
		std::string name = row[functions.eval];
		feedg->functions.erase(name);
		functionsList->erase(functionsList->get_iter(i.get_path()));
	}
}
