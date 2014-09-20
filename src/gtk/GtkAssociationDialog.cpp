#include <gtkmm/fixed.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>

#include "GtkAssociationDialog.hpp"
#include "GtkMainWindow.hpp"

GtkAssociationDialog::GtkAssociationDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	rbuilder->get_widget("mag", m_magnetChk );
	rbuilder->get_widget("tor", m_torrentChk);
}

void GtkAssociationDialog::on_response(int response)
{
	aWithMagnets = m_magnetChk->get_active();
	aWithTorrents = m_torrentChk->get_active();
}
