#include "GtkSettingsDialog.hpp"

#include <gtorrent/Settings.hpp>
#include <gtorrent/Platform.hpp>

#include <iostream>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/switch.h>
#include <gtkmm/builder.h>
#include <gtkmm/textview.h>

#include "../Application.hpp"
#include "GtkMainWindow.hpp"
#include "GtkTorrentTreeView.hpp"

using namespace std;

GtkSettingsDialog::GtkSettingsDialog(GtkMainWindow *Parent) : parent(Parent)
{
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_resource("/org/gtk/gtorrent/settings.glade");
	builder->get_widget("dialog1", dial);
	dial->set_modal();
	if(Parent) dial->set_transient_for(*Parent);
	backup = gt::Settings::settings;

	builder->get_widget(         "okbutt",         okbutt);
	builder->get_widget(       "forebutt",       forebutt);
	builder->get_widget(       "backbutt",       backbutt);
	builder->get_widget(       "textview",       textview);
	builder->get_widget(      "dhtswitch",      dhtswitch);
	builder->get_widget(     "chokecombo",     chokecombo);
	builder->get_widget(     "activdspin",     activdspin);
	builder->get_widget(     "activsspin",     activsspin);
	builder->get_widget(     "showtoggle",     showtoggle);
	builder->get_widget(     "anontoggle",     anontoggle);
	builder->get_widget(     "chokecombo",     chokecombo);
	builder->get_widget(     "ddashcheck",     ddashcheck);
	builder->get_widget(     "udashcheck",     udashcheck);
	builder->get_widget(     "filltoggle",     filltoggle);
	builder->get_widget(    "uplimitspin",        uplimit);
	builder->get_widget(    "increcispin",        increci);
	builder->get_widget(    "decrecispin",        decreci);
	builder->get_widget(    "notokaybutt",      notokbutt);
	builder->get_widget(    "savepathbox",    savepathbox);
	builder->get_widget(    "upcolorbutt",    upcolorbutt);
	builder->get_widget(    "statuscombo",    statuscombo);
	builder->get_widget(   "dhtlimitspin",       dhtlimit);
	builder->get_widget(   "useragentbox",   useragentbox);
	builder->get_widget(   "notifyswitch",   notifytoggle);
	builder->get_widget(  "gridcolorbutt",       gridbutt);
	builder->get_widget(  "downlimitspin",      downlimit);
	builder->get_widget(  "cachesizespin",      cachesize);
	builder->get_widget(  "downcolorbutt",  downcolorbutt);
	builder->get_widget(  "suggesttoggle",  suggesttoggle);
	builder->get_widget(  "overridecombo",  overridecombo);
	builder->get_widget( "seedchokecombo", seedchokecombo);
	builder->get_widget( "dcurvefillbutt", dcurvefillbutt);
	builder->get_widget("ucurvefillbutt",  ucurvefillbutt);
	builder->get_widget("filechooserbutt",       filebutt);
	builder->get_widget("cachechunksspin",     cachechunk);
	builder->get_widget("cacheexpiryspin",    cacheexpiry);
	builder->get_widget("defaultrecispin",    defaultreci);

	savepathbox->set_can_focus(false); //user has to use the filechooser to set the savepath

	gt::Settings::settings["CacheSize"                ] = gt::Settings::settings["CacheSize"                ].empty() ?  "512" : gt::Settings::settings["CacheSize"                ];
	gt::Settings::settings["CacheExpiry"              ] = gt::Settings::settings["CacheExpiry"              ].empty() ?   "10" : gt::Settings::settings["CacheExpiry"              ];
	gt::Settings::settings["ActiveSeeds"              ] = gt::Settings::settings["ActiveSeeds"              ].empty() ?    "4" : gt::Settings::settings["ActiveSeeds"              ];
	gt::Settings::settings["CachedChunks"             ] = gt::Settings::settings["CachedChunks"             ].empty() ?   "20" : gt::Settings::settings["CachedChunks"             ];
	gt::Settings::settings["DHTUploadLimit"           ] = gt::Settings::settings["DHTUploadLimit"           ].empty() ? "4000" : gt::Settings::settings["DHTUploadLimit"           ];
	gt::Settings::settings["ActiveDownloads"          ] = gt::Settings::settings["ActiveDownloads"          ].empty() ?    "8" : gt::Settings::settings["ActiveDownloads"          ];
	gt::Settings::settings["OverrideSettings"         ] = gt::Settings::settings["OverrideSettings"         ].empty() ?   "10" : gt::Settings::settings["OverrideSettings"         ];
	gt::Settings::settings["GlobalUploadLimit"        ] = gt::Settings::settings["GlobalUploadLimit"        ].empty() ?    "0" : gt::Settings::settings["GlobalUploadLimit"        ];
	gt::Settings::settings["GlobalDownloadLimit"      ] = gt::Settings::settings["GlobalDownloadLimit"      ].empty() ?    "0" : gt::Settings::settings["GlobalDownloadLimit"      ];
	gt::Settings::settings["DefaultReciprocationRate" ] = gt::Settings::settings["DefaultReciprocationRate" ].empty() ?   "14" : gt::Settings::settings["DefaultReciprocationRate" ];
	gt::Settings::settings["DecreaseReciprocationRate"] = gt::Settings::settings["DecreaseReciprocationRate"].empty() ?    "3" : gt::Settings::settings["DecreaseReciprocationRate"];
	gt::Settings::settings["IncreaseReciprocationRate"] = gt::Settings::settings["IncreaseReciprocationRate"].empty() ?   "10" : gt::Settings::settings["IncreaseReciprocationRate"];

	cachesize  ->get_adjustment()->configure(std::stoi(gt::Settings::settings["CacheSize"                ]), -1, 0x7FFFFFF, 10, 10, 10);
	cacheexpiry->get_adjustment()->configure(std::stoi(gt::Settings::settings["CacheExpiry"              ]), -1, 0x7FFFFFF, 10, 10, 10);
	activsspin ->get_adjustment()->configure(std::stoi(gt::Settings::settings["ActiveSeeds"              ]), -1, 0x7FFFFFF,  1,  1,  1);
	cachechunk ->get_adjustment()->configure(std::stoi(gt::Settings::settings["CachedChunks"             ]), -1, 0x7FFFFFF, 10, 10, 10);
	dhtlimit   ->get_adjustment()->configure(std::stoi(gt::Settings::settings["DHTUploadLimit"           ]), -1, 0x7FFFFFF, 10, 10, 10);
	activdspin ->get_adjustment()->configure(std::stoi(gt::Settings::settings["ActiveDownloads"          ]), -1, 0x7FFFFFF,  1,  1,  1);
	uplimit    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["GlobalUploadLimit"        ]), -1, 0x7FFFFFF, 10, 10, 10);
	downlimit  ->get_adjustment()->configure(std::stoi(gt::Settings::settings["GlobalDownloadLimit"      ]), -1, 0x7FFFFFF, 10, 10, 10);
	defaultreci->get_adjustment()->configure(std::stoi(gt::Settings::settings["DefaultReciprocationRate" ]), 0,        100, 10, 10, 10);
	decreci    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["DecreaseReciprocationRate"]), 0,        100, 10, 10, 10);
	increci    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["IncreaseReciprocationRate"]), 0,        100, 10, 10, 10);

	okbutt        ->signal_clicked      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onOkClicked         ));
	notokbutt     ->signal_clicked      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onnotOkClicked      ));
	statuscombo   ->signal_changed      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onStatusChanged     ));
	chokecombo    ->signal_changed      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onChokeChanged      ));
	overridecombo ->signal_changed      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onOverrideChanged   ));
	filebutt      ->signal_file_set     ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onFilebuttSet       ));
	forebutt      ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onForegroundSet     ));
	backbutt      ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onBackgroundSet     ));
	downcolorbutt ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownColorSet      ));
	upcolorbutt   ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpColorSet        ));
	gridbutt      ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onGridColorSet      ));
	dcurvefillbutt->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownFillColorSet  ));
	ucurvefillbutt->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpFillColorSet    ));
	activdspin    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onActivDChanged     ));
	activsspin    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onActivSChanged     ));
	uplimit       ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpLimitChanged    ));
	increci       ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onIncreciChanged    ));
	decreci       ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDecreciChanged    ));
	dhtlimit      ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDHTLimitChanged   ));
	downlimit     ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownLimitChanged  ));
	cachesize     ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheSizeChanged  ));
	cachechunk    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheChunkChanged ));
	cacheexpiry   ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheExpiryChanged));
	defaultreci   ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDefaultReciChanged));
}

int GtkSettingsDialog::run()
{
	dcurvefillbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphDownloadFillColor" ]));
	ucurvefillbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphUploadFillColor"   ]));
	upcolorbutt   ->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphUploadCurveColor"  ]));
	downcolorbutt ->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphDownloadCurveColor"]));
	gridbutt      ->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphGridColor"         ]));

	statuscombo->set_active(0);

	savepathbox ->set_text(gt::Settings::settings["SavePath" ]);
	useragentbox->set_text(gt::Settings::settings["UserAgent"]);

	std::string tmp = gt::Settings::settings["DHTBootstraps"];

	auto buff = textview->get_buffer();
	buff->set_text("");
	while(!tmp.empty())
	{
		buff->set_text(buff->get_text() + tmp.substr(0, tmp.find(',')) + "\n");
		if(tmp.find(',') == std::string::npos) break;
		tmp = tmp.substr(tmp.find(',') + 1);
	}
	
	dhtswitch    ->set_active(gt::Settings::settings["DHTEnabled"        ] == "Yes");
	showtoggle   ->set_active(gt::Settings::settings["ShowLegend"        ] == "Yes");
	anontoggle   ->set_active(gt::Settings::settings["AnonymousMode"     ] == "Yes");
	suggesttoggle->set_active(gt::Settings::settings["PieceSuggestion"   ] == "Yes");
	notifytoggle ->set_active(gt::Settings::settings["EnableNotification"] == "Yes");

	ddashcheck->set_active(gt::Settings::settings["GraphDownloadCurveStyle"] == "Dash");
	udashcheck->set_active(gt::Settings::settings["GraphUploadCurveStyle"  ] == "Dash");

	filltoggle   ->set_active(gt::Settings::settings["GraphStyle"        ] == "Fill");

	return dial->run();
}

void GtkSettingsDialog::onOkClicked()
{
	gt::Settings::settings["SavePath"       ] = savepathbox->get_text();
	gt::Settings::settings["UserAgent"      ] = useragentbox->get_text();
	gt::Settings::settings["GraphStyle"             ] = (filltoggle   ->get_active()) ? "Fill" : "Curves";
	gt::Settings::settings["DHTEnabled"             ] = (dhtswitch    ->get_active()) ?  "Yes" :     "No";
	gt::Settings::settings["ShowLegend"             ] = (showtoggle   ->get_active()) ?  "Yes" :     "No";
	gt::Settings::settings["AnonymousMode"          ] = (anontoggle   ->get_active()) ?  "Yes" :     "No";
	gt::Settings::settings["PieceSuggestion"        ] = (suggesttoggle->get_active()) ?  "Yes" :     "No";
	gt::Settings::settings["EnableNotification"     ] = (notifytoggle ->get_active()) ?  "Yes" :     "No";
	gt::Settings::settings["GraphUploadCurveStyle"  ] = (udashcheck   ->get_active()) ? "Dash" :   "Line";
	gt::Settings::settings["GraphDownloadCurveStyle"] = (ddashcheck   ->get_active()) ? "Dash" :   "Line";

	auto buff = textview->get_buffer();
	string tmp = buff->get_text();
	string routers;
	while(!tmp.empty())
	{
		tmp.erase(tmp.find_last_not_of(" \n\r\t")+1);
		routers += tmp.substr(0, tmp.find('\n'));
		if(tmp.find('\n') == std::string::npos) break; //ignore trailing newlines
		routers += ',';
		tmp = tmp.substr(tmp.find('\n') + 1);
	}
	gt::Settings::settings["DHTBootstraps"] = routers;
	backup = gt::Settings::settings;
	Application::getSingleton()->getCore()->setSessionParameters(); //reload settings
	dial->hide();
}

void GtkSettingsDialog::onnotOkClicked()
{
	gt::Settings::settings = backup;
	dial->hide();
}

void GtkSettingsDialog::onUpLimitChanged()
{
	gt::Settings::settings["GlobalUploadLimit"] = uplimit->get_text();
}

void GtkSettingsDialog::onDownLimitChanged()
{
	gt::Settings::settings["GlobalDownloadLimit"] = downlimit->get_text();
}

void GtkSettingsDialog::onDHTLimitChanged()
{
	gt::Settings::settings["DHTUploadLimit"] = dhtlimit->get_text();
}

void GtkSettingsDialog::onCacheSizeChanged()
{
	gt::Settings::settings["CacheSize"] = cachesize->get_text();
}

void GtkSettingsDialog::onCacheExpiryChanged()
{
	gt::Settings::settings["CacheExpiry"] = cacheexpiry->get_text();
}

void GtkSettingsDialog::onCacheChunkChanged()
{
	gt::Settings::settings["CachedChunks"] = cachechunk->get_text();
}

void GtkSettingsDialog::onDefaultReciChanged()
{
	gt::Settings::settings["DefaultReciprocationRate"] = defaultreci->get_text();
}

void GtkSettingsDialog::onIncreciChanged()
{
	gt::Settings::settings["IncreaseReciprocationRate"] = increci->get_text();
}

void GtkSettingsDialog::onDecreciChanged()
{
	gt::Settings::settings["DecreaseReciprocationRate"] = decreci->get_text();
}

void GtkSettingsDialog::onFilebuttSet()
{
	savepathbox->set_text(filebutt->get_filename());
}

void GtkSettingsDialog::onStatusChanged()
{
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["CheckingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["CheckingBackGroundColor"]));
		break;
	case 1:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["AllocatingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["AllocatingBackGroundColor"]));
		break;
	case 2:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["SeedingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["SeedingBackGroundColor"]));
		break;
	case 3:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["DownloadingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["DownloadingBackGroundColor"]));
		break;
	case 4:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedCheckingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedCheckingBackGroundColor"]));
		break;
	case 5:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["PausedForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["PausedBackGroundColor"]));
		break;
	case 6:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["MetadataForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["MetadataBackGroundColor"]));
		break;
	case 7:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["ResumingForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["ResumingBackGroundColor"]));
		break;
	case 8:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["FinishedForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["FinishedBackGroundColor"]));
		break;
	case 9:
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedBackGroundColor"]));
	}
}

void GtkSettingsDialog::onForegroundSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        forebutt->get_rgba().get_red_u()   / 256,
	        forebutt->get_rgba().get_green_u() / 256,
	        forebutt->get_rgba().get_blue_u()  / 256);
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		gt::Settings::settings["CheckingForeGroundColor"] = colorstring;
		break;
	case 1:
		gt::Settings::settings["AllocatingForeGroundColor"] = colorstring;
		break;
	case 2:
		gt::Settings::settings["SeedingForeGroundColor"] = colorstring;
		break;
	case 3:
		gt::Settings::settings["DownloadingForeGroundColor"] = colorstring;
		break;
	case 4:
		gt::Settings::settings["QueuedCheckingForeGroundColor"] = colorstring;
		break;
	case 5:
		gt::Settings::settings["PausedForeGroundColor"] = colorstring;
		break;
	case 6:
		gt::Settings::settings["MetadataForeGroundColor"] = colorstring;
		break;
	case 7:
		gt::Settings::settings["ResumingForeGroundColor"] = colorstring;
		break;
	case 8:
		gt::Settings::settings["FinishedForeGroundColor"] = colorstring;
		break;
	case 9:
		gt::Settings::settings["QueuedForeGroundColor"] = colorstring;
	}

	if(parent != nullptr)
		parent->m_treeview->reloadColors();
}

void GtkSettingsDialog::onBackgroundSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        backbutt->get_rgba().get_red_u()   / 256,
	        backbutt->get_rgba().get_green_u() / 256,
	        backbutt->get_rgba().get_blue_u()  / 256);
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		gt::Settings::settings["CheckingBackGroundColor"   ] = colorstring;
		break;
	case 1:
		gt::Settings::settings["AllocatingBackGroundColor" ] = colorstring;
		break;
	case 2:
		gt::Settings::settings["SeedingBackGroundColor"    ] = colorstring;
		break;
	case 3:
		gt::Settings::settings["DownloadingBackGroundColor"] = colorstring;
		break;
	case 4:
		gt::Settings::settings["QueuedCheckingBackGroundColor"] = colorstring;
		break;
	case 5:
		gt::Settings::settings["PausedBackGroundColor"     ] = colorstring;
		break;
	case 6:
		gt::Settings::settings["MetadataBackGroundColor"   ] = colorstring;
		break;
	case 7:
		gt::Settings::settings["ResumingBackGroundColor"   ] = colorstring;
		break;
	case 8:
		gt::Settings::settings["FinishedBackGroundColor"   ] = colorstring;
		break;
	case 9:
		gt::Settings::settings["QueuedBackGroundColor"] = colorstring;
	}

	if(parent != nullptr)
		parent->m_treeview->reloadColors();
}

void GtkSettingsDialog::onGridColorSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        gridbutt->get_rgba().get_red_u()   / 256,
	        gridbutt->get_rgba().get_green_u() / 256,
	        gridbutt->get_rgba().get_blue_u()  / 256);
	gt::Settings::settings["GraphBorderColor"] = colorstring;
	gt::Settings::settings["GraphHLineColor" ] = colorstring;
	gt::Settings::settings["GraphGridColor"  ] = colorstring;
}

void GtkSettingsDialog::onDownColorSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        downcolorbutt->get_rgba().get_red_u()   / 256,
	        downcolorbutt->get_rgba().get_green_u() / 256,
	        downcolorbutt->get_rgba().get_blue_u()  / 256);
	gt::Settings::settings["GraphDownloadCurveColor"] = colorstring;
}

void GtkSettingsDialog::onUpColorSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        upcolorbutt->get_rgba().get_red_u()   / 256,
	        upcolorbutt->get_rgba().get_green_u() / 256,
	        upcolorbutt->get_rgba().get_blue_u()  / 256);
	gt::Settings::settings["GraphUploadCurveColor"] = colorstring;
}

void GtkSettingsDialog::onChokeChanged()
{
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		gt::Settings::settings["ChokeAlgorithm"] = "Default";
		break;
	case 1:
		gt::Settings::settings["ChokeAlgorithm"] = "AutoExpand";
		break;
	case 2:
		gt::Settings::settings["ChokeAlgorithm"] = "RateBased";
		break;
	case 3:
		gt::Settings::settings["ChokeAlgorithm"] = "BitTyrant";
	}
}

void GtkSettingsDialog::onSeedChokeChanged()
{
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		gt::Settings::settings["SeedChokingAlgorithm"] = "RoundRobin";
		break;
	case 1:
		gt::Settings::settings["SeedChokingAlgorithm"] = "FastestUpload";
		break;
	case 2:
		gt::Settings::settings["SeedChokingAlgorithm"] = "AntiLeech";
	}
}

void GtkSettingsDialog::onOverrideChanged()
{
	switch(statuscombo->get_active_row_number())
	{
	case 0:
		gt::Settings::settings["OverrideSettings"] = "No";
		break;
	case 1:
		gt::Settings::settings["OverrideSettings"] = "Minimal";
		break;
	case 2:
		gt::Settings::settings["OverrideSettings"] = "HighPerformanceSeeding";
	}
}

void GtkSettingsDialog::onActivDChanged()
{
	gt::Settings::settings["ActiveDownloads"] = activdspin->get_text();
}

void GtkSettingsDialog::onActivSChanged()
{
	gt::Settings::settings["ActiveSeeds"] = activsspin->get_text();
}


void GtkSettingsDialog::onDownFillColorSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        dcurvefillbutt->get_rgba().get_red_u()   / 256,
	        dcurvefillbutt->get_rgba().get_green_u() / 256,
	        dcurvefillbutt->get_rgba().get_blue_u()  / 256);
	gt::Settings::settings["GraphDownloadFillColor"] = colorstring;
}

void GtkSettingsDialog::onUpFillColorSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        ucurvefillbutt->get_rgba().get_red_u()   / 256,
	        ucurvefillbutt->get_rgba().get_green_u() / 256,
	        ucurvefillbutt->get_rgba().get_blue_u()  / 256);
	gt::Settings::settings["GraphUploadFillColor"] = colorstring;
}
