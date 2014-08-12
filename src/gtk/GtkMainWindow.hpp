#pragma once

#include <glibmm.h>
#include <giomm.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/main.h>
#include <gtkmm/stock.h>
#include <boost/algorithm/string.hpp>
#include <gtkmm/uimanager.h>
#include <gtkmm/paned.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/scrolledwindow.h>

#include <gtorrent/Core.hpp>

#include "GtkTorrentTreeView.hpp"
#include "GtkTorrentInfoBar.hpp"
#include "GtkSettingsDialog.hpp"

//#include <Application.hpp>


class GtkMainWindow : public Gtk::Window
{
	shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *d = nullptr;

	Gtk::HeaderBar *header;
	// Gtk::Statusbar *status;

	// Signal Responders

	void onAboutBtnClicked();
	void onAddBtnClicked();
	void onAddMagnetBtnClicked();
	void onPauseBtnClicked();
	void onResumeBtnClicked();
	void onRemoveBtnClicked();
	void onSettingsBtnClicked();
	void onPropertiesBtnClicked();
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

public:
	GtkTorrentTreeView *m_treeview;
	GtkTorrentInfoBar  *m_infobar;
	Gtk::ScrolledWindow *m_swin;
	GtkMainWindow();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();

	Gtk::MenuButton *btn_add_link = Gtk::manage(new Gtk::MenuButton());
	Gtk::Entry      *magtxt       = Gtk::manage(new Gtk::Entry());
	//We've Always Been Shameless About Stealing Great Ideas
	Gtk::Popover    *magPop = Gtk::manage(new Gtk::Popover(*btn_add_link)); // We may need to provide a fallback CSS for those who use old themes
	// thx prince
	Gtk::Button *btn_add_torrent  = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_pause        = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_properties   = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_remove       = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_resume       = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_settings     = Gtk::manage(new Gtk::Button());

};
