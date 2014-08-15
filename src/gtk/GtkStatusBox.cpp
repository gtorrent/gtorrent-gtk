#include "GtkStatusBox.hpp"
#include <gtkmm/hvseparator.h>
#include <gtorrent/Torrent.hpp>

GtkStatusBox::GtkStatusBox()
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 5)
{

    // Transfer
	m_transfer = Gtk::manage(new Gtk::Table(8, 3, false));
	m_transfer->set_col_spacings(5);
    // Line 1
    Gtk::Label *m_time_elapsed_label = Gtk::manage(new Gtk::Label());
	m_time_elapsed_label->set_markup("<b>Time Elapsed:</b>");
	m_time_elapsed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_time_elapsed_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_time_elapsed, 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 0, 1, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_remaining_label = Gtk::manage(new Gtk::Label());
	m_remaining_label->set_markup("<b>Remaining:</b>");
	m_remaining = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_remaining_label, 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_remaining, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_share_ratio_label = Gtk::manage(new Gtk::Label());
	m_share_ratio_label->set_markup("<b>Ratio:</b>");
	m_share_ratio = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_share_ratio_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_share_ratio, 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);

    // Line 2
    Gtk::Label *m_downloaded_label = Gtk::manage(new Gtk::Label());
	m_downloaded_label->set_markup("<b>Downloaded:</b>");
	m_downloaded = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_downloaded_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_downloaded, 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 1, 2, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_download_speed_label = Gtk::manage(new Gtk::Label());
	m_download_speed_label->set_markup("<b>Download Speed:</b>");
	m_download_speed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_download_speed_label, 3, 4, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_download_speed, 4, 5, 1, 2, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 1, 2, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_down_limit_label = Gtk::manage(new Gtk::Label());
	m_down_limit_label->set_markup("<b>Down Limit:</b>");
	m_down_limit = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_down_limit_label, 6, 7, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_down_limit, 7, 8, 1, 2, Gtk::AttachOptions::SHRINK);

    // Line 3
    Gtk::Label *m_uploaded_label = Gtk::manage(new Gtk::Label());
	m_uploaded_label->set_markup("<b>Uploaded:</b>");
	m_uploaded = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_uploaded_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_uploaded, 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);
	
    m_transfer->attach(*(new Gtk::VSeparator()), 2, 3, 2, 3, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_upload_speed_label = Gtk::manage(new Gtk::Label());
	m_upload_speed_label->set_markup("<b>Upload Speed:</b>");
	m_upload_speed = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_upload_speed_label, 3, 4, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_upload_speed, 4, 5, 2, 3, Gtk::AttachOptions::SHRINK);

    m_transfer->attach(*(new Gtk::VSeparator()), 5, 6, 2, 3, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_up_limit_label = Gtk::manage(new Gtk::Label());
	m_up_limit_label->set_markup("<b>Up Limit:</b>");
	m_up_limit = Gtk::manage(new Gtk::Label());
	m_transfer->attach(*m_up_limit_label, 6, 7, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer->attach(*m_up_limit, 7, 8, 2, 3, Gtk::AttachOptions::SHRINK);

    // Tracker
	m_tracker = Gtk::manage(new Gtk::Table(5, 1, false));
	m_tracker->set_col_spacings(5);

    // Line 1
    Gtk::Label *m_tracker_url_label = Gtk::manage(new Gtk::Label());
	m_tracker_url_label->set_markup("<b>Tracker URL:</b>");
	m_tracker_url = Gtk::manage(new Gtk::Label());
	m_tracker->attach(*m_tracker_url_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_tracker->attach(*m_tracker_url, 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

    m_tracker->attach(*(new Gtk::VSeparator()), 2, 3, 0, 1, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_tracker_status_label = Gtk::manage(new Gtk::Label());
	m_tracker_status_label->set_markup("<b>Tracker Status:</b>");
	m_tracker_status = Gtk::manage(new Gtk::Label());
	m_tracker->attach(*m_tracker_status_label, 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);
	m_tracker->attach(*m_tracker_status, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);

    // General
	m_general = Gtk::manage(new Gtk::Table(5, 3, false));
	m_general->set_col_spacings(5);

    // Line 1
    Gtk::Label *m_save_as_label = Gtk::manage(new Gtk::Label());
	m_save_as_label->set_markup("<b>Save As:</b>");
	m_save_as = Gtk::manage(new Gtk::Label());
	m_general->attach(*m_save_as_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_general->attach(*m_save_as, 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

    // Line 2
    Gtk::Label *m_total_size_label = Gtk::manage(new Gtk::Label());
	m_total_size_label->set_markup("<b>Total Size:</b>");
	m_total_size = Gtk::manage(new Gtk::Label());
	m_general->attach(*m_total_size_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general->attach(*m_total_size, 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);

    m_general->attach(*(new Gtk::VSeparator()), 2, 3, 1, 2, Gtk::AttachOptions::EXPAND);

    Gtk::Label *m_pieces_label = Gtk::manage(new Gtk::Label());
	m_pieces_label->set_markup("<b>Pieces:</b>");
	m_pieces = Gtk::manage(new Gtk::Label());
	m_general->attach(*m_pieces_label, 3, 4, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general->attach(*m_pieces, 4, 5, 1, 2, Gtk::AttachOptions::SHRINK);

    // Line 3
    Gtk::Label *m_hash_label = Gtk::manage(new Gtk::Label());
	m_hash_label->set_markup("<b>Hash:</b>");
	m_hash = Gtk::manage(new Gtk::Label());
	m_general->attach(*m_hash_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general->attach(*m_hash, 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);


    // Box setup
    Gtk::Label *m_transfer_label = Gtk::manage(new Gtk::Label());
	m_transfer_label->set_markup("<b><big>Transfer</big></b>");
    Gtk::Label *m_tracker_label = Gtk::manage(new Gtk::Label());
	m_tracker_label->set_markup("<b><big>Tracker</big></b>");
    Gtk::Label *m_general_label = Gtk::manage(new Gtk::Label());
	m_general_label->set_markup("<b><big>General</big></b>");
    this->add(*m_transfer_label);
    this->add(*m_transfer);
    this->add(*m_tracker_label);
    this->add(*m_tracker);
    this->add(*m_general_label);
    this->add(*m_general);

}

void GtkStatusBox::update(std::shared_ptr<gt::Torrent> selected)
{
    m_time_elapsed->set_text(selected->getTextActiveTime());
    m_remaining->set_text(selected->getTextEta());
    m_share_ratio->set_text(std::to_string(selected->getTotalRatio()));

    m_downloaded->set_text(selected->getTextTotalDownloaded());
    m_download_speed->set_text(selected->getTextDownloadRate());
    m_down_limit->set_text("");

    m_uploaded->set_text(selected->getTextTotalUploaded());
    m_upload_speed->set_text(selected->getTextUploadRate());
    m_up_limit->set_text("");

    m_tracker_url->set_text(selected->getCurrentTrackerURL());
    m_tracker_status->set_text("");

    m_save_as->set_text(selected->getSavePath());

    m_total_size->set_text(selected->getTextSize());
    m_pieces->set_text(std::to_string(selected->getPieces().size()));

    m_hash->set_text(selected->getFormattedHash());
}
