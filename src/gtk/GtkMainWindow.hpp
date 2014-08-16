#pragma once

#include <gtorrent/Core.hpp>

#include <gtkmm/menubutton.h>
#include <gtkmm/window.h>
#include <gtkmm/popover.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>

namespace Gtk
{
	class HeaderBar;
	class Window;
	class Button;
	class Popover;
	class ScrolledWindow;
	class FileChooserDialog;
	class Paned;
}

class GtkTorrentInfoBar;
class GtkTorrentTreeView;
class GtkSettingsDialog;

class GtkMainWindow : public Gtk::Window
{
	std::shared_ptr<gt::Core> &m_core;
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
