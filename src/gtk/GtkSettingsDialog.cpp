#include "GtkSettingsDialog.hpp"
#include <gdkmm/color.h>
#include <gdkmm/rgba.h>
#include <iostream>
#include <gtkmm/builder.h>
#include <gtkmm/adjustment.h>
#include <Settings.hpp>
#include "GtkMainWindow.hpp"

using namespace std;

GtkSettingsDialog::GtkSettingsDialog(GtkMainWindow *Parent) : parent(Parent)
{
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file(gt::Platform::getExecutablePath().substr(0, gt::Platform::getExecutablePath().find_last_of('/') + 1) +  "settings.glade");
	builder->get_widget("dialog1", dial);
	dial->set_modal();
	backup = gt::Settings::settings;

	builder->get_widget(         "okbutt", okbutt);
	builder->get_widget(       "forebutt", forebutt);
	builder->get_widget(       "backbutt", backbutt);
	builder->get_widget(     "showtoggle", showtoggle);
	builder->get_widget(     "anontoggle", anontoggle);
	builder->get_widget(     "chokecombo", chokecombo);
	builder->get_widget(    "increcispin", increci);
	builder->get_widget(    "decrecispin", decreci);
	builder->get_widget(    "statuscombo", statuscombo);
	builder->get_widget(    "notokaybutt", notokbutt);
	builder->get_widget(    "uplimitspin", uplimit);
	builder->get_widget(    "savepathbox", savepathbox);
	builder->get_widget(   "useragentbox", useragentbox);
	builder->get_widget(   "dhtlimitspin", dhtlimit);
	builder->get_widget(  "downlimitspin", downlimit);
	builder->get_widget(  "cachesizespin", cachesize);
	builder->get_widget(  "suggesttoggle", suggesttoggle);
	builder->get_widget( "seedchokecombo", seedchokecombo);
	builder->get_widget("cacheexpiryspin", cacheexpiry);
	builder->get_widget("cachechunksspin", cachechunk);
	builder->get_widget("defaultrecispin", defaultreci);
	builder->get_widget("filechooserbutt", filebutt);
	builder->get_widget(  "gridcolorbutt", gridbutt);
	builder->get_widget(    "upcolorbutt", upcolorbutt);
	builder->get_widget(  "downcolorbutt", downcolorbutt);
	builder->get_widget(  "chokecombo", chokecombo);
	savepathbox->set_can_focus(false); //user has to use the filechooser to set the savepath
	gt::Settings::settings["GlobalUploadLimit"]         = gt::Settings::settings["GlobalUploadLimit"]         .empty() ?    "0" : gt::Settings::settings["GlobalUploadLimit"];
	gt::Settings::settings["DecreaseReciprocationRate"] = gt::Settings::settings["DecreaseReciprocationRate"] .empty() ?    "3" : gt::Settings::settings["DecreaseReciprocationRate"];
	gt::Settings::settings["IncreaseReciprocationRate"] = gt::Settings::settings["IncreaseReciprocationRate"] .empty() ?   "10" : gt::Settings::settings["IncreaseReciprocationRate"];
	gt::Settings::settings["DHTUploadLimit"]            = gt::Settings::settings["DHTUploadLimit"]            .empty() ? "4000" : gt::Settings::settings["DHTUploadLimit"];
	gt::Settings::settings["GlobalDownloadLimit"]       = gt::Settings::settings["GlobalDownloadLimit"]       .empty() ?    "0" : gt::Settings::settings["GlobalDownloadLimit"];
	gt::Settings::settings["CacheSize"]                 = gt::Settings::settings["CacheSize"]                 .empty() ?  "512" : gt::Settings::settings["CacheSize"];
	gt::Settings::settings["CachedChunks"]              = gt::Settings::settings["CachedChunks"]              .empty() ?   "20" : gt::Settings::settings["CachedChunks"];
	gt::Settings::settings["DefaultReciprocationRate"]  = gt::Settings::settings["DefaultReciprocationRate"]  .empty() ?   "14" : gt::Settings::settings["DefaultReciprocationRate"];
	gt::Settings::settings["CacheExpiry"]               = gt::Settings::settings["CacheExpiry"]               .empty() ?   "10" : gt::Settings::settings["CacheExpiry"];
	decreci    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["DecreaseReciprocationRate"]),  0, 100, 10, 10, 10);
	increci    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["IncreaseReciprocationRate"]),  0, 100, 10, 10, 10);
	defaultreci->get_adjustment()->configure(std::stoi(gt::Settings::settings["DefaultReciprocationRate"]),   0, 100, 10, 10, 10);
	uplimit    ->get_adjustment()->configure(std::stoi(gt::Settings::settings["GlobalUploadLimit"]),         -1, 0x7FFFFFF, 10, 10, 10);
	dhtlimit   ->get_adjustment()->configure(std::stoi(gt::Settings::settings["DHTUploadLimit"]),            -1, 0x7FFFFFF, 10, 10, 10);
	downlimit  ->get_adjustment()->configure(std::stoi(gt::Settings::settings["GlobalDownloadLimit"]),       -1, 0x7FFFFFF, 10, 10, 10);
	cachesize  ->get_adjustment()->configure(std::stoi(gt::Settings::settings["CacheSize"]),                 -1, 0x7FFFFFF, 10, 10, 10);
	cachechunk ->get_adjustment()->configure(std::stoi(gt::Settings::settings["CachedChunks"]),              -1, 0x7FFFFFF, 10, 10, 10);
	cacheexpiry->get_adjustment()->configure(std::stoi(gt::Settings::settings["CacheExpiry"]),               -1, 0x7FFFFFF, 10, 10, 10);
	okbutt       ->signal_clicked      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onOkClicked));
	notokbutt    ->signal_clicked      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onnotOkClicked));
	statuscombo  ->signal_changed      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onStatusChanged));
	chokecombo   ->signal_changed      ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onChokeChanged));
	filebutt     ->signal_file_set     ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onFilebuttSet));
	forebutt     ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onForegroundSet));
	backbutt     ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onBackgroundSet));
	downcolorbutt->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownColorSet));
	upcolorbutt  ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpColorSet));
	gridbutt     ->signal_color_set    ().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onGridColorSet));
	uplimit      ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpLimitChanged));
	increci      ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onIncreciChanged));
	decreci      ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDecreciChanged));
	dhtlimit     ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDHTLimitChanged));
	downlimit    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownLimitChanged));
	cachesize    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheSizeChanged));
	cachechunk   ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheChunkChanged));
	cacheexpiry  ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheExpiryChanged));
	defaultreci  ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDefaultReciChanged));

}

int GtkSettingsDialog::run()
{
	upcolorbutt  ->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphUploadCurveColor"]));
	downcolorbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphDownloadCurveColor"]));
	gridbutt     ->set_rgba(Gdk::RGBA(gt::Settings::settings["GraphGridColor"]));

	savepathbox->set_text(gt::Settings::settings["SavePath"]);
	statuscombo->set_active(0);
	useragentbox->set_text(gt::Settings::settings["UserAgent"]);

	showtoggle   ->set_active(gt::Settings::settings["ShowLegend"] == "Yes");
	anontoggle   ->set_active(gt::Settings::settings["AnonymousMode"] == "Yes");
	suggesttoggle->set_active(gt::Settings::settings["PieceSuggestion"] == "Yes");

	return dial->run();
}

void GtkSettingsDialog::onOkClicked()
{
	gt::Settings::settings["SavePath"]        = savepathbox->get_text();
	gt::Settings::settings["UserAgent"]       = useragentbox->get_text();
	gt::Settings::settings["ShowLegend"]      = (showtoggle   ->get_active()) ? "Yes" : "No";
	gt::Settings::settings["AnonymousMode"]   = (anontoggle   ->get_active()) ? "Yes" : "No";
	gt::Settings::settings["PieceSuggestion"] = (suggesttoggle->get_active()) ? "Yes" : "No";

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
		forebutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedForeGroundColor"]));
		backbutt->set_rgba(Gdk::RGBA(gt::Settings::settings["QueuedBackGroundColor"]));
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
	}
}

void GtkSettingsDialog::onForegroundSet()
{
	char colorstring[16] = {0};
	sprintf(colorstring, "#%02X%02X%02X",
	        forebutt->get_rgba().get_red_u()   / 256,
	        forebutt->get_rgba().get_green_u() / 256,
	        forebutt->get_rgba().get_blue_u()  / 256);
	std::cout << colorstring << std::endl;
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
		gt::Settings::settings["QueuedForeGroundColor"] = colorstring;
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
		gt::Settings::settings["CheckingBackGroundColor"] = colorstring;
		break;
	case 1:
		gt::Settings::settings["AllocatingBackGroundColor"] = colorstring;
		break;
	case 2:
		gt::Settings::settings["SeedingBackGroundColor"] = colorstring;
		break;
	case 3:
		gt::Settings::settings["DownloadingBackGroundColor"] = colorstring;
		break;
	case 4:
		gt::Settings::settings["QueuedBackGroundColor"] = colorstring;
		break;
	case 5:
		gt::Settings::settings["PausedBackGroundColor"] = colorstring;
		break;
	case 6:
		gt::Settings::settings["MetadataBackGroundColor"] = colorstring;
		break;
	case 7:
		gt::Settings::settings["ResumingBackGroundColor"] = colorstring;
		break;
	case 8:
		gt::Settings::settings["FinishedBackGroundColor"] = colorstring;
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
	gt::Settings::settings["GraphGridColor"] = colorstring;
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
