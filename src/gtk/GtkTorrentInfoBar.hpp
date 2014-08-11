#pragma once

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/table.h>

#include <gtorrent/Torrent.hpp>

#include "../Application.hpp"
#include "GtkBlockBar.hpp"
#include "GtkGraph.hpp"

class GtkTorrentInfoBar : public Gtk::Box
{
private:
	GtkBlockBar *m_progress;
	Gtk::Label *m_title;
	GtkGraph *m_graph;
	Gtk::Notebook *m_notebook;
	Gtk::Box *m_stat_box;
	Gtk::Box *m_piece_box;
	Gtk::Box 			*m_network_box;

	Gtk::Table 		*m_info_table_layout;

	Gtk::Label 		*m_eta_label;
	Gtk::Label 		*m_eta;
	Gtk::Label 		*m_active_time_label;
	Gtk::Label 		*m_active_time;
	Gtk::Label 		*m_seeding_time_label;
	Gtk::Label 		*m_seeding_time;
	Gtk::Label 		*m_last_scrape_label;
	Gtk::Label 		*m_last_scrape;
	Gtk::Label 		*m_info_hash_label;
	Gtk::Label 		*m_info_hash;
	Gtk::Label 		*m_save_path_label;
	Gtk::Label 		*m_save_path;
	Gtk::Label 		*m_total_wanted_label;
	Gtk::Label 		*m_total_wanted;
	Gtk::Label 		*m_total_wanted_done_label;
	Gtk::Label 		*m_total_wanted_done;
	Gtk::Label 		*m_all_time_download_label;
	Gtk::Label 		*m_all_time_download;
	Gtk::Label 		*m_all_time_upload_label;
	Gtk::Label 		*m_all_time_upload;
	Gtk::Label 		*m_progress_ppm_label;
	Gtk::Label 		*m_progress_ppm;
	Gtk::Label 		*m_paused_label;
	Gtk::Label 		*m_paused;
	Gtk::Label 		*m_is_finished_label;
	Gtk::Label 		*m_is_finished;
	Gtk::Label 		*m_is_seeding_label;
	Gtk::Label 		*m_is_seeding;
	Gtk::Label 		*m_auto_managed_label;
	Gtk::Label 		*m_auto_managed;
	Gtk::Label 		*m_sequential_download_label;
	Gtk::Label 		*m_sequential_download;
	Gtk::Label 		*m_ip_filter_applies_label;
	Gtk::Label 		*m_ip_filter_applies;
	Gtk::Label 		*m_has_metadata_label;
	Gtk::Label 		*m_has_metadata;
	Gtk::Label 		*m_has_incoming_label;
	Gtk::Label 		*m_has_incoming;

	Gtk::Table 		*m_network_table_layout;

	Gtk::Label 		*m_state_label;
	Gtk::Label 		*m_state;
	Gtk::Label 		*m_current_tracker_label;
	Gtk::Label 		*m_current_tracker;
	Gtk::Label 		*m_seed_rank_label;
	Gtk::Label 		*m_seed_rank;
	Gtk::Label 		*m_download_rate_label;
	Gtk::Label 		*m_download_rate;
	Gtk::Label 		*m_upload_rate_label;
	Gtk::Label 		*m_upload_rate;
	Gtk::Label 		*m_num_uploads_label;
	Gtk::Label 		*m_num_uploads;
	Gtk::Label 		*m_num_connections_label;
	Gtk::Label 		*m_num_connections;
	Gtk::Label 		*m_connect_candidates_label;
	Gtk::Label 		*m_connect_candidates;
	Gtk::Label 		*m_num_complete_label;
	Gtk::Label 		*m_num_complete;
	Gtk::Label 		*m_num_seeds_label;
	Gtk::Label 		*m_num_seeds;
	Gtk::Label 		*m_num_incomplete_label;
	Gtk::Label 		*m_num_incomplete;
	Gtk::Label 		*m_num_peers_label;
	Gtk::Label 		*m_num_peers;
	Gtk::Label 		*m_distributed_copies_label;
	Gtk::Label 		*m_distributed_copies;
public:
	GtkTorrentInfoBar();
	void updateInfo(shared_ptr<gt::Torrent> selected);
};
