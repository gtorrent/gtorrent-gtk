#pragma once

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

#include <gtorrent/Core.hpp>

#include "GtkTorrentTreeView.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkAddMagnetLinkWindow.hpp"
#include "GtkSettingsDialog.hpp"

//#include <Application.hpp>


class GtkMainWindow : public Gtk::Window
{
	shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *d = nullptr;

	Gtk::HeaderBar *header;

	// Signal Responders

	void onAddBtnClicked();
	void onAddMagnetBtnClicked();
	void onPauseBtnClicked();
	void onResumeBtnClicked();
	void onRemoveBtnClicked();
	void onSettingsBtnClicked();
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

public:
	GtkTorrentTreeView *m_treeview;
	GtkTorrentInfoBar  *m_infobar;
	GtkMainWindow();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};
