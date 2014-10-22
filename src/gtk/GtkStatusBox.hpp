#pragma once

#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/builder.h>
#include "../Application.hpp"

class GtkStatusBox : public Gtk::Box
{
public:
	const Glib::RefPtr<Gtk::Builder> builder;
	GtkStatusBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> rbuilder);
	void update(std::shared_ptr<gt::Torrent> selected);
private:

	Gtk::Label *m_time_elapsed;
	Gtk::Label *m_share_ratio;
	Gtk::Label *m_remaining;
	Gtk::Label *m_downloaded;
	Gtk::Label *m_download_speed;
	Gtk::Label *m_down_limit;
	Gtk::Label *m_uploaded;
	Gtk::Label *m_upload_speed;
	Gtk::Label *m_up_limit;
	Gtk::Label *m_tracker_url;
	Gtk::Label *m_tracker_status;
	Gtk::Label *m_save_as;
	Gtk::Label *m_total_size;
	Gtk::Label *m_pieces;
	Gtk::Label *m_hash;
};
