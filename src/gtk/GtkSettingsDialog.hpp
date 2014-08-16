#pragma once

#include <map>
//#include <gtkmm.h>

namespace Gtk
{
	class Button;
	class Dialog;
	class FileChooserButton;
	class ComboBoxText;
	class Switch;
	class ColorButton;
	class Entry;
	class SpinButton;
	class Application;
}

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
