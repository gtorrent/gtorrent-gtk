#pragma once

#include <gtorrent/Core.hpp>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popover.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/window.h>
#include <gtkmm/separator.h>
#include <gtkmm/paned.h>

class GtkTorrentInfoBar;
class GtkTorrentTreeView;
class GtkSettingsDialog;

class GtkMainWindow : public Gtk::Window
{
	const Glib::RefPtr<Gtk::Builder> builder;
	std::shared_ptr<gt::Core> &m_core;
	GtkSettingsDialog *d = nullptr;

	// Signal Responders
	void onAboutBtnClicked();
	void onAddBtnClicked();
	void onAddMagnetBtnClicked();
	void onPauseBtnClicked();
	void onResumeBtnClicked();
	void onRemoveBtnClicked();
	void onSettingsBtnClicked();
	void onPropertiesBtnClicked();
	void torrentStateChangedCallback(int oldstate, std::shared_ptr<gt::Torrent> t);
	void onFileDropped(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

public:
	GtkTorrentTreeView *m_treeview = nullptr;
	GtkTorrentInfoBar  *m_infobar = nullptr;

	Gtk::Button *addTorrentButton = nullptr, *resumeButton = nullptr, *pauseButton = nullptr, *removeButton = nullptr, *propertiesButton = nullptr, *settingsButton = nullptr;
	Gtk::Separator *vSeparatorOne = nullptr, *vSeparatorTwo = nullptr; 
	Gtk::ScrolledWindow *scrolledWindow = nullptr;
	Gtk::MenuButton *addMagnetButton = nullptr;
	Gtk::Paned *panel = nullptr;


//	Gtk::ScrolledWindow *m_swin;

/*	Gtk::MenuButton *btn_add_link = Gtk::manage(new Gtk::MenuButton());
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
*/

	GtkMainWindow(GtkWindow*, const Glib::RefPtr<Gtk::Builder>);
	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};
