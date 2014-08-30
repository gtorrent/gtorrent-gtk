#include <gtkmm/fixed.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/dialog.h>

#include "GtkAssociationDialog.hpp"
#include "GtkMainWindow.hpp"

GtkAssociationDialog::GtkAssociationDialog(GtkDialog *dial, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Dialog(dial)
{
	Gtk::Fixed *panel = Gtk::manage(new Gtk::Fixed());
	m_magnetChk = Gtk::manage(new Gtk::CheckButton("Associate with magnets"));
	m_torrentChk = Gtk::manage(new Gtk::CheckButton("Associate with .torrent files"));
	m_magnetChk ->set_active();
	m_torrentChk->set_active();

	panel->put( *m_magnetChk, 15, 0);
	panel->put(*m_torrentChk, 15, 20);
	Gtk::Box *content = get_vbox();
//	Gtk::ButtonBox *action = get_action_area();

	content->pack_end(*panel, true, true);
	content->show_all();

	// No fun allowed
	add_button("Remind me later", -1);
	Gtk::Button *okButton = add_button("Ok", 0);
	okButton->signal_released().connect(sigc::bind(sigc::mem_fun(*this, &GtkAssociationDialog::PlsFindMeAName), okButton));
	rbuilder->get_widget("mag", m_magnetChk );
	rbuilder->get_widget("tor", m_torrentChk);

}

void GtkAssociationDialog::on_response(int response)
{
	aWithMagnets = m_magnetChk->get_active();
	aWithTorrents = m_torrentChk->get_active();
}
