#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>

class GtkMainWindow;

class GtkRSSDialog : public Gtk::Dialog
{
	// TODO: Add controls pointers here
public:
	GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder>) { /* TODO: Actually implement the contructor here */};
	virtual void on_response(int response) {}; // TODO: implement this
};
