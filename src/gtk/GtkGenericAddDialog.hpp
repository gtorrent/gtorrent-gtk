#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/entry.h>

class GtkGenericAddDialog : public Gtk::Dialog
{
public:
	Gtk::Label *label1 = nullptr, *label2 = nullptr;
	Gtk::Entry *entry1 = nullptr, *entry2 = nullptr;
	GtkGenericAddDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder);
};
