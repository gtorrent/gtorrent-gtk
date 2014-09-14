#pragma once

#include <memory>
#include <gtorrent/FeedGroup.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>

class GtkFunctionDialog : public Gtk::Dialog
{
	std::shared_ptr<gt::FeedGroup> feedg;
public:
	Gtk::Entry        *constant = nullptr;
	Gtk::ComboBoxText   *filter = nullptr, *operation = nullptr;
	GtkFunctionDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder>);
	void setFeedGroup(std::shared_ptr<gt::FeedGroup> fg);
};
