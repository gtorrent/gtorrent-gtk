#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/builder.h>

namespace Gtk
{
        class CheckButton;
        class Button;
}

class GtkMainWindow;

class GtkAssociationDialog : public Gtk::Dialog
{
	Gtk::CheckButton *m_magnetChk, *m_torrentChk;
public:
	GtkAssociationDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder>);
	virtual void on_response(int response);
	bool aWithMagnets, aWithTorrents;
};
