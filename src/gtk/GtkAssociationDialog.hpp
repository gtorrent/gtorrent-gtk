#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include "GtkMainWindow.hpp"

class GtkAssociationDialog : public Gtk::Dialog
{
private:
    Gtk::CheckButton *m_magnetChk;
    Gtk::CheckButton *m_torrentChk;
public:
	GtkAssociationDialog(GtkMainWindow &Parent);
	void PlsFindMeAName(Gtk::Button *butt);
};
