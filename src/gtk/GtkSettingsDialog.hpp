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
	class TextView;
}

class GtkMainWindow;

class GtkSettingsDialog
{
	Gtk::Dialog   *dial   = nullptr;
	GtkMainWindow *parent = nullptr;
	std::map<std::string, std::string> backup;

<<<<<<< HEAD
	Gtk::SpinButton        *uplimit,        *downlimit,  *dhtlimit,      *cachesize,     *cacheexpiry,  *cachechunk,     *defaultreci, *increci, *decreci, *activdspin, *activsspin;
	Gtk::Entry             *useragentbox,   *hostbox,    *portbox,       *userbox,       *passbox,      *savepathbox;
	Gtk::ColorButton       *forebutt,       *backbutt,   *gridbutt,      *downcolorbutt, *upcolorbutt,  *dcurvefillbutt, *ucurvefillbutt;
	Gtk::Switch            *showtoggle,     *anontoggle, *suggesttoggle, *filltoggle,    *notifytoggle, *dhtswitch;
=======
	Gtk::SpinButton        *uplimit,        *downlimit,  *dhtlimit,      *cachesize,     *cacheexpiry, *cachechunk,     *defaultreci, *increci, *decreci, *activdspin, *activsspin;
	Gtk::Entry             *useragentbox,   *hostbox,    *portbox,       *userbox,       *passbox,     *savepathbox;
	Gtk::ColorButton       *forebutt,       *backbutt,   *gridbutt,      *downcolorbutt, *upcolorbutt, *dcurvefillbutt, *ucurvefillbutt;
	Gtk::Switch            *showtoggle,     *anontoggle, *suggesttoggle, *filltoggle,    *notifytoggle,*gridtoggle;
>>>>>>> origin/master
	Gtk::ComboBoxText      *seedchokecombo, *chokecombo, *statuscombo,   *overridecombo;
	Gtk::Button            *okbutt,         *notokbutt;
	Gtk::CheckButton       *ddashcheck,     *udashcheck;
	Gtk::FileChooserButton *filebutt;
	Gtk::TextView          *textview;

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
