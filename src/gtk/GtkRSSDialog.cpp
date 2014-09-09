#include "GtkRSSDialog.hpp"

GtkRSSDialog::GtkRSSDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder)
{
/*
	rbuilder->get_widget(    "cancelButton",     cancelButton);
	rbuilder->get_widget(        "okButton",         okButton);
	rbuilder->get_widget(   "addFeedButton",    addFeedButton);
	rbuilder->get_widget("removeFeedButton", removeFeedButton);
	rbuilder->get_widget(      "aTogButton",       aTogButton);
	rbuilder->get_widget(      "gToaButton",       gToaButton);
	rbuilder->get_widget(    "addFilterBtn",     addFilterBtn);
	rbuilder->get_widget( "removeFilterBtn",  removeFilterBtn);
	rbuilder->get_widget(    "addFunButton",        addFunBtn);
	rbuilder->get_widget( "removeFunButton",     removeFunBtn);

	cancelButton    ->signal_clicked().connect([this](){});
	okButton        ->signal_clicked().connect([this](){});
	addFeedButton   ->signal_clicked().connect([this](){});
	removeFeedButton->signal_clicked().connect([this](){});
	aTogButton      ->signal_clicked().connect([this](){});
	gToaButton      ->signal_clicked().connect([this](){});
	addFilterBtn    ->signal_clicked().connect([this](){});
	removeFilterBtn ->signal_clicked().connect([this](){});
	addFunBtn       ->signal_clicked().connect([this](){});
	removeFunBtn    ->signal_clicked().connect([this](){});
*/
}

void GtkRSSDialog::on_response(int response)
{
}
