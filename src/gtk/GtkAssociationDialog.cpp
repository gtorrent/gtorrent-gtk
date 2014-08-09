#include <gtkmm/fixed.h>
#include <gtkmm/box.h>


#include "GtkAssociationDialog.hpp"

GtkAssociationDialog::GtkAssociationDialog(GtkMainWindow &Parent) : Gtk::Dialog("File Association", Parent, true)
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

	add_button("no", -1);
	Gtk::Button *okButton = add_button("yes", 0);
	okButton->signal_released().connect(sigc::bind(sigc::mem_fun(*this, &GtkAssociationDialog::PlsFindMeAName), okButton));
}

void GtkAssociationDialog::PlsFindMeAName(Gtk::Button *butt)
{
	response(m_magnetChk->get_active() << 1 | m_torrentChk->get_active());
}
