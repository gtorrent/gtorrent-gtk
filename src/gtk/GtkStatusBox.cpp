#include "GtkStatusBox.hpp"
#include <gtkmm/hvseparator.h>

GtkStatusBox::GtkStatusBox()
{
	m_transfer = Gtk::manage(new Gtk::Table(8, 3, false));
	m_transfer->set_col_spacings(5);
    this->add(*m_transfer);

    Gtk::Label *m_time_elapsed_label = Gtk::manage(new Gtk::Label(""));
	m_time_elapsed_label->set_markup("<b>Time Elapsed:</b>");
	m_time_elapsed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_time_elapsed_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_time_elapsed, 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 0, 1, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_remaining_label = Gtk::manage(new Gtk::Label(""));
	m_remaining_label->set_markup("<b>Remaining:</b>");
	m_remaining = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_remaining_label, 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_remaining, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_share_ratio_label = Gtk::manage(new Gtk::Label(""));
	m_share_ratio_label->set_markup("<b>Ratio:</b>");
	m_share_ratio = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_share_ratio_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_share_ratio, 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);
}
    
void update(shared_ptr<gt::Torrent> selected)
{
    m_time_elapsed->set_text(selected->getActiveTimeString());
    m_remaining->set_text(selected->getEtaString());
    m_share_ratio->set_text(selected->getTotalRatioString());
}
