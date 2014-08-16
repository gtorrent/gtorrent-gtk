#pragma once

#include <map>

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/switch.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/colorbutton.h>
#include "../Application.hpp"

class GtkMainWindow;

class GtkSettingsDialog
{
	Gtk::Dialog *dial = nullptr;
	GtkMainWindow *parent = nullptr;
	std::map<std::string, std::string> backup;

	Gtk::SpinButton        *uplimit,        *downlimit,  *dhtlimit, *cachesize,     *cacheexpiry, *cachechunk, *defaultreci, *increci, *decreci;//
	Gtk::Entry             *useragentbox,   *hostbox,    *portbox,  *userbox,       *passbox,     *savepathbox;//
	Gtk::ColorButton       *forebutt,       *backbutt,   *gridbutt, *downcolorbutt, *upcolorbutt;
	Gtk::ComboBoxText      *seedchokecombo, *chokecombo, *statuscombo;
	Gtk::Switch            *showtoggle,     *anontoggle, *suggesttoggle; //
	Gtk::Button            *okbutt,         *notokbutt;//
	Gtk::FileChooserButton *filebutt;//

public:
	GtkSettingsDialog(GtkMainWindow *Parent = nullptr);
	void onOkClicked();
	int run();

	void onnotOkClicked();
	void onUpLimitChanged();
	void onDownLimitChanged();
	void onDHTLimitChanged();
	void onCacheSizeChanged();
	void onCacheExpiryChanged();
	void onCacheChunkChanged();
	void onDefaultReciChanged();
	void onIncreciChanged();
	void onDecreciChanged();
	void onFilebuttSet();
	void onStatusChanged();

	void onForegroundSet();
	void onBackgroundSet();
	void onGridColorSet();
	void onDownColorSet();
	void onUpColorSet();
	void onChokeChanged();
	void onSeedChokeChanged();
};
