#pragma once

#include "GtkTorrentTreeView.hpp"
#include <gtkmm/headerbar.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtorrent/Core.hpp>
#include <gtkmm/main.h>

class GtkMainWindow : public Gtk::Window
{
private:
	shared_ptr<gt::Core> &m_core;

	Gtk::HeaderBar *header;
	GtkTorrentTreeView *m_treeview;
	// Gtk::Statusbar *status;

	// Signal Responders

	void onAboutBtnClicked();
	void onAddBtnClicked();
	void onAddMagnetBtnClicked();
	void onPauseBtnClicked();
	void onResumeBtnClicked();
	void onRemoveBtnClicked();
	void onPropertiesBtnClicked();
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

public:
	GtkMainWindow();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};
