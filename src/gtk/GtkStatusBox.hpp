#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include "../Application.hpp"
	
class GtkStatusBox : public Gtk::Grid
{
    private:

    Gtk::Label m_transfer;
    // Line 1
    Gtk::Label m_time_elapsed_label;
    Gtk::Label m_time_elapsed;
    Gtk::Label m_share_ratio_label;
    Gtk::Label m_share_ratio;
    Gtk::Label m_remaining_label;
    Gtk::Label m_remaining;
    // Line 2
    Gtk::Label m_downloaded_label;
    Gtk::Label m_downloaded;
    Gtk::Label m_download_speed_label;
    Gtk::Label m_download_speed;
    Gtk::Label m_down_limit_label;
    Gtk::Label m_down_limit;
    // Line 3
    Gtk::Label m_uploaded_label;
    Gtk::Label m_uploaded;
    Gtk::Label m_upload_speed_label;
    Gtk::Label m_upload_speed;
    Gtk::Label m_up_limit_label;
    Gtk::Label m_up_limit;

    Gtk::Label m_tracker;
    // Line 1
    Gtk::Label m_tracker_url_label;
    Gtk::Label m_tracker_url;
    Gtk::Label m_tracker_status_label;
    Gtk::Label m_tracker_status;

    Gtk::Label m_general;
    // Line 1
    Gtk::Label m_save_as_label;
    Gtk::Label m_save_as;
    // Line 2
    Gtk::Label m_total_size_label;
    Gtk::Label m_total_size;
    Gtk::Label m_pieces_label;
    Gtk::Label m_pieces;
    // Line 3
    Gtk::Label m_hash_label;
    Gtk::Label m_hash;

    public:
    GtkStatusBox();
    void update(std::shared_ptr<gt::Torrent> selected);
};
