#include "GtkGenericAddDialog.hpp"

GtkGenericAddDialog::GtkGenericAddDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	rbuilder->get_widget("label1", label1);
	rbuilder->get_widget("label2", label2);
	rbuilder->get_widget("entry1", entry1);
	rbuilder->get_widget("entry2", entry2);
}
