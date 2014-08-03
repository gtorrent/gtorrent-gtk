#pragma once

#include <gtkmm/headerbar.h>
#include <gtkmm/textview.h>
#include <gtkmm/dialog.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

#include <gtorrent/Core.hpp>

class GtkAddRssWindow : public Gtk::Dialog
{
private:
	Gtk::HeaderBar *m_header;
	Gtk::TextView *m_textview;

	// Signal Responders

	void onAddBtnClicked();
	void onClipboardReady(const Glib::ustring &text);

public:
	GtkAddRssWindow();

	// Getters

	string getRssURL();
};
