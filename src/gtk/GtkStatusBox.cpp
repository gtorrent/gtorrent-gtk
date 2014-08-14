#include "GtkStatusBox.hpp"
#include <gtkmm/hvseparator.h>
#include <gtorrent/Torrent.hpp>

GtkStatusBox::GtkStatusBox()
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 5)
{
	m_transfer = Gtk::manage(new Gtk::Table(8, 4, false));
	m_transfer->set_col_spacings(5);

    Gtk::Label *m_transfer_label = Gtk::manage(new Gtk::Label());
	m_transfer_label->set_markup("<b><big>Transfer</big></b>");
    this->add(*m_transfer_label);
    this->add(*m_transfer);

    // Line 1
    Gtk::Label *m_time_elapsed_label = Gtk::manage(new Gtk::Label());
	m_time_elapsed_label->set_markup("<b>Time Elapsed:</b>");
	m_time_elapsed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_time_elapsed_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_time_elapsed, 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 1, 2, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_remaining_label = Gtk::manage(new Gtk::Label());
	m_remaining_label->set_markup("<b>Remaining:</b>");
	m_remaining = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_remaining_label, 3, 4, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_remaining, 4, 5, 1, 2, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 1, 2, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_share_ratio_label = Gtk::manage(new Gtk::Label());
	m_share_ratio_label->set_markup("<b>Ratio:</b>");
	m_share_ratio = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_share_ratio_label, 6, 7, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_share_ratio, 7, 8, 1, 2, Gtk::AttachOptions::SHRINK);

    // Line 2
    Gtk::Label *m_downloaded_label = Gtk::manage(new Gtk::Label());
	m_downloaded_label->set_markup("<b>Downloaded:</b>");
	m_downloaded = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_downloaded_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_downloaded, 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 2, 3, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_download_speed_label = Gtk::manage(new Gtk::Label());
	m_download_speed_label->set_markup("<b>Download Speed:</b>");
	m_download_speed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_download_speed_label, 3, 4, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_download_speed, 4, 5, 2, 3, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 2, 3, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_down_limit_label = Gtk::manage(new Gtk::Label());
	m_down_limit_label->set_markup("<b>Down Limit:</b>");
	m_down_limit = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_down_limit_label, 6, 7, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_down_limit, 7, 8, 2, 3, Gtk::AttachOptions::SHRINK);

    // Line 3
    Gtk::Label *m_uploaded_label = Gtk::manage(new Gtk::Label());
	m_uploaded_label->set_markup("<b>Uploaded:</b>");
	m_uploaded = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_uploaded_label, 0, 1, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_uploaded, 1, 2, 3, 4, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 3, 4, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_upload_speed_label = Gtk::manage(new Gtk::Label());
	m_upload_speed_label->set_markup("<b>Upload Speed:</b>");
	m_upload_speed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_upload_speed_label, 3, 4, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_upload_speed, 4, 5, 3, 4, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 3, 4, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_up_limit_label = Gtk::manage(new Gtk::Label());
	m_up_limit_label->set_markup("<b>Up Limit:</b>");
	m_up_limit = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_up_limit_label, 6, 7, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_up_limit, 7, 8, 3, 4, Gtk::AttachOptions::SHRINK);
}

void GtkStatusBox::update(shared_ptr<gt::Torrent> selected)
{
    m_time_elapsed->set_text(std::to_string(selected->getActiveTime()));
    m_remaining->set_text(std::to_string(selected->getEta()));
    m_share_ratio->set_text(std::to_string(selected->getTotalRatio()));

    m_downloaded->set_text(std::to_string(selected->getTotalDownloaded()));
    m_download_speed->set_text(std::to_string(selected->getDownloadRate()));

    m_uploaded->set_text(std::to_string(selected->getTotalUploaded()));
    m_upload_speed->set_text(std::to_string(selected->getUploadRate()));
}
