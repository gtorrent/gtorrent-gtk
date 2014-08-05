#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/switch.h>
#include <gtkmm/combobox.h>
#include <map>

class GtkSettingsDialog
{
	Gtk::Dialog *dial = nullptr;
	std::map<std::string, std::string> backup;
	Gtk::Button *okbutt, *notokbutt;
	Gtk::SpinButton *uplimit, *downlimit, *dhtlimit, *cachesize, *cacheexpiry, *cachechunk, *defaultreci, *increci, *decreci;
	Gtk::Entry *useragentbox, *hostbox, *portbox, *userbox, *passbox, *savepathbox;
	Gtk::ComboBox *seedchokecombo, *chokecombo, *statuscombo;
	Gtk::Switch *showtoggle, *anontoggle, *suggesttoggle;
	
public:
	GtkSettingsDialog();
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

};
