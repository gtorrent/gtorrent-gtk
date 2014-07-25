#pragma once

#include <gtorrent/Core.hpp>

#include <glibmm.h>
#include <giomm.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <boost/algorithm/string.hpp>
#include <gtkmm/uimanager.h>
#include <gtkmm/paned.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>

#include "GtkTorrentTreeView.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkAddMagnetLinkWindow.hpp"

//#include <Application.hpp>


class GtkMainWindow : public Gtk::Window
{
private:
	shared_ptr<gt::Core> &m_core;

	Gtk::HeaderBar *header;
	GtkTorrentTreeView *m_treeview;
	GtkTorrentInfoBar *m_infobar;
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
