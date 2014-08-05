#include <iostream>
#include "GtkSettingsDialog.hpp"
#include <gtkmm/builder.h>
#include <gtkmm/adjustment.h>
#include <Settings.hpp>

using namespace std;

GtkSettingsDialog::GtkSettingsDialog()
{
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("settings.glade");
	builder->get_widget("dialog1", dial);
	dial->set_modal();
	backup = gt::Settings::settings;

	builder->get_widget(         "okbutt", okbutt);
	builder->get_widget(     "showtoggle", showtoggle);
	builder->get_widget(     "anontoggle", anontoggle);
	builder->get_widget(    "increcispin", increci);
	builder->get_widget(    "decrecispin", decreci);
	builder->get_widget(    "notokaybutt", notokbutt);
	builder->get_widget(    "uplimitspin", uplimit);
	builder->get_widget(   "dhtlimitspin", dhtlimit);
	builder->get_widget(  "downlimitspin", downlimit);
	builder->get_widget(  "cachesizespin", cachesize);
	builder->get_widget(  "suggesttoggle", suggesttoggle);
	builder->get_widget("cacheexpiryspin", cacheexpiry);
	builder->get_widget("cachechunksspin", cachechunk);
	builder->get_widget("defaultrecispin", defaultreci);

	std::cout << "123" << std::endl;

	backup["GlobalUploadLimit"]         == "" ? "0" : backup["GlobalUploadLimit"];
	backup["DecreaseReciprocationRate"] == "" ? "0" : backup["DecreaseReciprocationRate"];
	backup["IncreaseReciprocationRate"] == "" ? "0" : backup["IncreaseReciprocationRate"];
	backup["DHTUploadLimit"]            == "" ? "0" : backup["DHTUploadLimit"];
	backup["GlobalDownloadLimit"]       == "" ? "0" : backup["GlobalDownloadLimit"];
	backup["CacheSize"]                 == "" ? "0" : backup["CacheSize"];
	backup["CachedChunks"]              == "" ? "0" : backup["CachedChunks"];
	backup["DefaultReciprocationRate"]  == "" ? "0" : backup["DefaultReciprocationRate"];
	backup["CacheExpiry"]               == "" ? "0" : backup["CacheExpiry"];
	
	std::cout << "456" << std::endl;

	uplimit    ->get_adjustment()->configure(std::stoi(backup["GlobalUploadLimit"]),         0, 0x7FFFFFF, 10, 10, 10);
	decreci    ->get_adjustment()->configure(std::stoi(backup["DecreaseReciprocationRate"]), 0, 0x7FFFFFF, 10, 10, 10);
	increci    ->get_adjustment()->configure(std::stoi(backup["IncreaseReciprocationRate"]), 0, 0x7FFFFFF, 10, 10, 10); 
	dhtlimit   ->get_adjustment()->configure(std::stoi(backup["DHTUploadLimit"]),            0, 0x7FFFFFF, 10, 10, 10); 
	downlimit  ->get_adjustment()->configure(std::stoi(backup["GlobalDownloadLimit"]),       0, 0x7FFFFFF, 10, 10, 10); 
	std::cout << "789" << std::endl;
	cachesize  ->get_adjustment()->configure(std::stoi(backup["CacheSize"]),                 0, 0x7FFFFFF, 10, 10, 10); 
	cachechunk ->get_adjustment()->configure(std::stoi(backup["CachedChunks"]),              0, 0x7FFFFFF, 10, 10, 10); 
	defaultreci->get_adjustment()->configure(std::stoi(backup["DefaultReciprocationRate"]),  0, 0x7FFFFFF, 10, 10, 10); 
	cacheexpiry->get_adjustment()->configure(std::stoi(backup["CacheExpiry"]),               0, 0x7FFFFFF, 10, 10, 10); 

	okbutt     ->signal_clicked().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onOkClicked));
	notokbutt  ->signal_clicked().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onnotOkClicked));	

	uplimit    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onUpLimitChanged)); 
	increci    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onIncreciChanged)); 
	decreci    ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDecreciChanged));
	dhtlimit   ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDHTLimitChanged)); 
	downlimit  ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDownLimitChanged)); 
	cachesize  ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheSizeChanged)); 
	cachechunk ->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheChunkChanged)); 
	cacheexpiry->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onCacheExpiryChanged)); 
	defaultreci->signal_value_changed().connect(sigc::mem_fun(*this, &GtkSettingsDialog::onDefaultReciChanged)); 

}

int GtkSettingsDialog::run()
{
	return dial->run();
}
	
void GtkSettingsDialog::onOkClicked()
{
	backup["ShowLegend"]      = (showtoggle   ->get_active()) ? "Yes" : "No";
	backup["AnonymousMode"]   = (anontoggle   ->get_active()) ? "Yes" : "No";
	backup["PieceSuggestion"] = (suggesttoggle->get_active()) ? "Yes" : "No";
	dial->hide();
}
	
void GtkSettingsDialog::onnotOkClicked()
{
	dial->hide();
}

void GtkSettingsDialog::onUpLimitChanged()
{
	try
	{
		backup["GlobalUploadLimit"] = stoi(uplimit->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onDownLimitChanged()
{
	try
	{
		backup["GlobalDownloadLimit"] = stoi(downlimit->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onDHTLimitChanged()
{
	try
	{
		backup["DHTUploadLimit"] = stoi(dhtlimit->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onCacheSizeChanged()
{
	try
	{
		backup["CacheSize"] = stoi(cachesize->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onCacheExpiryChanged()
{
	try
	{
		backup["CacheExpiry"] = stoi(cacheexpiry->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onCacheChunkChanged()
{
	try
	{
		backup["CachedChunks"] = stoi(cachechunk->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onDefaultReciChanged()
{
	try
	{
		backup["DefaultReciprocationRate"] = stoi(defaultreci->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onIncreciChanged()
{
	try
	{
		backup["IncreaseReciprocationRate"] = stoi(increci->get_text());
	}
	catch(...) {}
}
void GtkSettingsDialog::onDecreciChanged()
{
	try
	{
		backup["DecreaseReciprocationRate"] = stoi(decreci->get_text());
	}
	catch(...) {}
}
