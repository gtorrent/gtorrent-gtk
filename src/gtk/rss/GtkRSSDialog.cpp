#include "GtkRSSDialog.hpp"
#include "../GtkGenericAddDialog.hpp"
#include "../GtkFunctionDialog.hpp"
#include "../../Application.hpp"

// TODO: Rename "Functions" to "Criterions" or equivalent ?

GtkRSSDialog::GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	m_core = Application::getSingleton()->getCore();
	rbuilder->get_widget("cancelButton", cancelButton);
	rbuilder->get_widget("okButton", okButton);
	rbuilder->get_widget("addButton", addFeedButton);
	rbuilder->get_widget("rssTreeView", rssTreeView);
	set_default_response(1);

	this->show_all_children();
}

int GtkRSSDialog::run(std::string fName)
{
	// If run is called with a valid parameter, then we edit the passed feed, else we create a new
	// Returns a pointer to existing one if one with same name exists
	m_feedgroups = m_core->addFeedGroup(fName);

	set_title(fName);

	storeRssItems = Gtk::ListStore::create(items);
	rssTreeView->set_model(storeRssItems);
	rssTreeView->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	rssTreeView->append_column("test",   items.name);

	addFeedButton   ->signal_clicked().connect([this](){ feedAdd();});

	for(auto f : m_feedgroups->m_feeds)
	{
		Gtk::TreeRow row = *storeActiveFeeds->append();
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
		for(auto item : f->get_feed_status().items)
		{
			Gtk::TreeRow row = *storeRssItems->append();
			row[items.name] = item.title;
			row[items.item] = item;
		}
	}

	for(auto f : m_feedgroups->filters)
	{
		Gtk::TreeRow row = *storeFilters->append();
		row[filters.name]  = f.first;
		row[filters.regex] = f.second;
	}

	for(auto f : m_feedgroups->functions)
	{
		Gtk::TreeRow row = *storeFunctions->append();
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

void GtkRSSDialog::feedAdd()
{
	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/dialog_generic.ui");
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
			Gtk::TreeRow row = *storeGlobalFeeds->append();
			row[global.name] = addFilterDialog->entry1->get_text();
			auto f = m_core->addFeed(addFilterDialog->entry2->get_text());
			row[global.feed] = f;
			f->owners.insert(m_feedgroups);
		}
	}
	delete addFilterDialog;
}

// there's no constraint in removing a feed from the active list nor the global list since filters don't depend on them
void GtkRSSDialog::feedRemove()
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
		std::shared_ptr<gt::Feed> f = (*storeGlobalFeeds->get_iter(i.get_path()))[global.feed];
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
		storeGlobalFeeds->erase(globalTreeView->get_model()->get_iter(i.get_path()));
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
		moving.push_back((*storeGlobalFeeds->get_iter(i.get_path()))[global.feed]);
		storeGlobalFeeds->erase(globalTreeView->get_model()->get_iter(i.get_path()));
	}
	for(auto f : moving)
	{
		Gtk::TreeRow row = *storeActiveFeeds->append();
		m_feedgroups->m_feeds.insert(f);
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
		for(auto item : f->get_feed_status().items)
		{
			Gtk::TreeRow row = *storeRssItems->append();
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
		rows.push_back(Gtk::TreeModel::RowReference(storeActiveFeeds, path));
	for (auto i : rows)
	{
		std::shared_ptr<gt::Feed> f = (*storeActiveFeeds->get_iter(i.get_path()))[active.feed];
		moving.push_back(f);
		for(auto item : f->get_feed_status().items)
		{
			for(auto row : storeRssItems->children())
				if(item.url == libtorrent::feed_item(row[items.item]).url)
				{
					storeRssItems->erase(row);
					break;
				}
		}
		storeActiveFeeds->erase(storeActiveFeeds->get_iter(i.get_path()));
	}

	for(auto f : moving)
	{
		Gtk::TreeRow row = *storeGlobalFeeds->append();
		f->owners.erase(m_feedgroups);
		if(f->get_feed_status().title != "")
			row[active.name] = f->get_feed_status().title;
		else
			row[active.name] = f->get_feed_status().url;
		row[active.feed] = f;
	}
}

void GtkRSSDialog::filterAdd()
{
	// filters are run on every items from every active feed
	// Filter have uniques names, but can have same regexes
	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/dialog_generic.ui");
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
			Gtk::TreeRow row = *storeFilters->append();
			m_feedgroups->filters[addFilterDialog->entry1->get_text()] = addFilterDialog->entry2->get_text();
			row[filters.name] = addFilterDialog->entry1->get_text();
			row[filters.regex] = addFilterDialog->entry2->get_text();
		}
	}
	delete addFilterDialog;
}

void GtkRSSDialog::filterRemove()
{
	// You can remove a filter only if it's  not used by any function
	// if the user tries to remove an used filter, he should be prompted 
	// if he wants the functions removed or cancel the operation
	std::vector<std::shared_ptr<gt::Feed>> removing;
	auto sel   = filterTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(storeFilters, path));

	for (auto i : rows)
	{
		Gtk::TreeRow row = *storeFilters->get_iter(i.get_path());
		std::string name = row[filters.name];
		for(auto fun : m_feedgroups->functions)
			if(fun.find(name) != std::string::npos)
				return; // if filter is used in function, dont delete it // TODO: show an error dialog here
		m_feedgroups->filters.erase(name);
		storeFilters->erase(storeFilters->get_iter(i.get_path()));
	}
}

void GtkRSSDialog::functionAdd()
{
	// you can add a function only if the the filter specified in the lhs exists
	// if the specified filter doesn't exist, the user should warned, and if he
	// wants to proceed, a "New Filter" dialog will be shown before adding the function
	if(m_feedgroups->filters.empty()) {
		auto errorDial = std::make_shared<Gtk::MessageDialog>("You need to add filters in order to add functions !");
		errorDial->run();
		return;
	}

	auto refBuilder = Gtk::Builder::create();
	refBuilder->add_from_resource("/org/gtk/gtorrent/dialog_function.ui");
	GtkFunctionDialog *addFunctionDialog = nullptr;
	refBuilder->get_widget_derived("functiondialog", addFunctionDialog);
	addFunctionDialog->set_transient_for(*this);
	addFunctionDialog->set_default_response(1);
	addFunctionDialog->set_title("Function");
	addFunctionDialog->setFeedGroup(m_feedgroups);

	if(addFunctionDialog->run() == 1) {
		Gtk::TreeRow row = *storeFunctions->append();
		std::string fun = addFunctionDialog->filter->get_active_text();
		int op = addFunctionDialog->filter->get_active_row_number();
		std::string ops = "<>=!";
		fun += " " + ops[op] + ' ';
		fun += addFunctionDialog->constant->get_text();
		m_feedgroups->functions.insert(fun);
		row[functions.eval] = fun;
	}
	delete addFunctionDialog;
}

void GtkRSSDialog::toggleAutoAdd()
{
	m_feedgroups->autoAddNewItem = rssAuto->get_active();
}

void GtkRSSDialog::functionRemove()
{
	std::vector<std::shared_ptr<gt::Feed>> removing;
	auto sel   = funTreeView->get_selection();
	auto paths = sel->get_selected_rows();
	std::vector<Gtk::TreeModel::RowReference> rows;

	for (auto path : paths)
		rows.push_back(Gtk::TreeModel::RowReference(storeFunctions, path));

	for (auto i : rows) {
		Gtk::TreeRow row = *storeFunctions->get_iter(i.get_path());
		std::string name = row[functions.eval];
		m_feedgroups->functions.erase(name);
		storeFunctions->erase(storeFunctions->get_iter(i.get_path()));
	}
}

void GtkRSSDialog::setupTreeviews()
{

}
