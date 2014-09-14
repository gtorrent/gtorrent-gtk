#include "GtkFunctionDialog.hpp"

GtkFunctionDialog::GtkFunctionDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	rbuilder->get_widget("constant"  , constant);
	rbuilder->get_widget("filter"    , filter);
	rbuilder->get_widget("comparison", operation);
}

void GtkFunctionDialog::setFeedGroup(std::shared_ptr<gt::FeedGroup> fg)
{
	feedg = fg;
	for(auto f : fg->filters)
		filter->append(f.first);
}
