#pragma once

#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include "../Application.hpp"
	
class GtkStatusBox : public Gtk::Box
{
    private:
    Gtk::Table *m_transfer;
    // Line 1
    Gtk::Label *m_time_elapsed;
    Gtk::Label *m_remaining;
    Gtk::Label *m_share_ratio;
    // Line 2
    Gtk::Label *m_downloaded;
    Gtk::Label *m_download_speed;
    Gtk::Label *m_down_limit;
    // Line 3
    Gtk::Label *m_uploaded;
    Gtk::Label *m_upload_speed;
    Gtk::Label *m_up_limit;

    public:
    GtkStatusBox();
    void update();
};
