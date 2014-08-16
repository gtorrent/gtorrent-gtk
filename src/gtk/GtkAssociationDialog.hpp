#pragma once

#include <gtkmm/dialog.h>

namespace Gtk
{
        class CheckButton;
        class Button;
}

class GtkMainWindow;

class GtkAssociationDialog : public Gtk::Dialog
{
private:
	Gtk::CheckButton *m_magnetChk;
	Gtk::CheckButton *m_torrentChk;
public:
	GtkAssociationDialog(GtkMainWindow &Parent);
	void PlsFindMeAName(Gtk::Button *butt);
};
