#pragma once

#include <map>

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
	class CheckButton;
}

class GtkMainWindow;

class GtkSettingsDialog
{
	Gtk::Dialog   *dial   = nullptr;
	GtkMainWindow *parent = nullptr;
	std::map<std::string, std::string> backup;

	Gtk::SpinButton        *uplimit,        *downlimit,  *dhtlimit,      *cachesize,     *cacheexpiry,  *cachechunk,     *defaultreci,    *increci, *decreci, *activdspin, *activsspin;
	Gtk::Entry             *useragentbox,   *hostbox,    *portbox,       *userbox,       *passbox,      *savepathbox;
	Gtk::ColorButton       *forebutt,       *backbutt,   *gridbutt,      *downcolorbutt, *upcolorbutt,  *dcurvefillbutt, *ucurvefillbutt;
	Gtk::Switch            *showtoggle,     *anontoggle, *suggesttoggle, *filltoggle,    *notifytoggle, *gridtoggle    , *polylinetoggle, *rssnotifytoggle;
	Gtk::ComboBoxText      *seedchokecombo, *chokecombo, *statuscombo,   *overridecombo;
	Gtk::Button            *okbutt,         *notokbutt;
	Gtk::CheckButton       *ddashcheck,     *udashcheck;
	Gtk::FileChooserButton *filebutt;

public:
	GtkSettingsDialog(GtkMainWindow *Parent = nullptr);
	int run();

	void onOkClicked         ();
	void onUpColorSet        ();
	void onFilebuttSet       ();
	void onGridColorSet      ();
	void onDownColorSet      ();
	void onChokeChanged      ();
	void onnotOkClicked      ();
	void onStatusChanged     ();
	void onActivSChanged     ();
	void onActivDChanged     ();
	void onForegroundSet     ();
	void onBackgroundSet     ();
	void onIncreciChanged    ();
	void onUpLimitChanged    ();
	void onDecreciChanged    ();
	void onUpFillColorSet    ();
	void onOverrideChanged   ();
	void onDHTLimitChanged   ();
	void onCacheSizeChanged  ();
	void onSeedChokeChanged  ();
	void onDownLimitChanged  ();
	void onDownFillColorSet  ();
	void onCacheChunkChanged ();
	void onCacheExpiryChanged();
	void onDefaultReciChanged();
};
