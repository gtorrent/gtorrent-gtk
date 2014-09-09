#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>
#include <gtkmm/treeview.h>

class GtkMainWindow;

class GtkRSSDialog : public Gtk::Dialog
{
	Gtk::Button   *cancelButton = nullptr, *okButton = nullptr, *addFeedButton = nullptr, *removeFeedButton = nullptr, *aTogButton = nullptr, *gToaButton = nullptr, *addFilterBtn = nullptr, *removeFilterBtn = nullptr, *addFunBtn = nullptr, *removeFunBtn = nullptr;
	Gtk::TreeView *globalTree = nullptr, *activeTreeView = nullptr, *filterTreeView = nullptr, *funTreeView = nullptr;
public:
	GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder);
	virtual void on_response(int response); // TODO: implement this
};
