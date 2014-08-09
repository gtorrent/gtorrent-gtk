#pragma once

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/table.h>

#include <gtorrent/Torrent.hpp>

#include "../Application.hpp"
//#include "GtkFiles.hpp"
#include "GtkGraph.hpp"
//#include "GtkOverview.hpp"
//#include "GtkPeers.hpp"
#include "GtkPiece.hpp"
//#include "GtkPreferences.hpp"
//#include "GtkTrackers.hpp"


class GtkTorrentInfoBar : public Gtk::Box
{
	private:

		Gtk::Label 		*m_title;
		Gtk::Notebook *m_notebook;
		Gtk::Box 			*m_overview_box;
		GtkGraph 			*m_graph;
		Gtk::Box 			*m_stat_box;
		Gtk::Box 			*m_times_box;
		Gtk::Box 			*m_bools_box;
		Gtk::Box 			*m_piece_box;
		GtkPiece 			*m_progress;
		Gtk::Box 			*m_files_box;
		Gtk::Box 			*m_peers_box;
		Gtk::Box 			*m_trackers_box;

		Gtk::Table 		*m_table_layout;
		Gtk::Table 		*m_bools_table_layout;
		Gtk::Table 		*m_times_table_layout;

		Gtk::Label 		*m_down_total_label;
		Gtk::Label 		*m_down_total;

		Gtk::Label 		*m_up_total_label;
		Gtk::Label 		*m_up_total;

		Gtk::Label 		*m_error_label;
   	Gtk::Label 		*m_error;

	  Gtk::Label 		*m_save_path_label;
		Gtk::Label 		*m_save_path;

	 	Gtk::Label 		*m_name_label;
		Gtk::Label 		*m_name;

		Gtk::Label 		*m_torrent_file_label;
		Gtk::Label 		*m_torrent_file;

		Gtk::Label 		*m_next_announce_label;
	  Gtk::Label 		*m_next_announce;

		Gtk::Label 		*m_announce_interval_label;
	  Gtk::Label 		*m_announce_interval;

		Gtk::Label 		*m_current_tracker_label;
		Gtk::Label 		*m_current_tracker;

		Gtk::Label 		*m_total_download_label;
		Gtk::Label 		*m_total_download;

		Gtk::Label 		*m_total_upload_label;
		Gtk::Label 		*m_total_upload;

		Gtk::Label 		*m_total_payload_download_label;
	  Gtk::Label 		*m_total_payload_download;

		Gtk::Label 		*m_total_payload_upload_label;
		Gtk::Label 		*m_total_payload_upload;

		Gtk::Label 		*m_total_failed_bytes_label;
		Gtk::Label 		*m_total_failed_bytes;

		Gtk::Label 		*m_total_redundant_bytes_label;
		Gtk::Label 		*m_total_redundant_bytes;

		Gtk::Label 		*m_total_done_label;
	  Gtk::Label 		*m_total_done;

		Gtk::Label 		*m_total_wanted_done_label;
	  Gtk::Label 		*m_total_wanted_done;

		Gtk::Label 		*m_total_wanted_label;
	  Gtk::Label 		*m_total_wanted;

		Gtk::Label 		*m_all_time_upload_label;
	  Gtk::Label 		*m_all_time_upload;

		Gtk::Label 		*m_all_time_download_label;
	  Gtk::Label 		*m_all_time_download;

		Gtk::Label 		*m_added_time_label;
		Gtk::Label 		*m_added_time;

		Gtk::Label 		*m_completed_time_label;
		Gtk::Label 		*m_completed_time;

		Gtk::Label 		*m_last_seen_complete_label;
		Gtk::Label 		*m_last_seen_complete;

		Gtk::Label 		*m_storage_mode_label;
		Gtk::Label 		*m_storage_mode;

	 	Gtk::Label 		*m_progress_ppm_label;
	  Gtk::Label 		*m_progress_ppm;

		Gtk::Label 		*m_queue_position_label;
	  Gtk::Label 		*m_queue_position;

		Gtk::Label 		*m_download_rate_label;
	  Gtk::Label 		*m_download_rate;

		Gtk::Label 		*m_upload_rate_label;
	  Gtk::Label 		*m_upload_rate;

		Gtk::Label 		*m_download_payload_rate_label;
	  Gtk::Label 		*m_download_payload_rate;

		Gtk::Label 		*m_upload_payload_rate_label;
	  Gtk::Label 		*m_upload_payload_rate;

		Gtk::Label 		*m_num_seeds_label;
	  Gtk::Label 		*m_num_seeds;

		Gtk::Label 		*m_num_peers_label;
		Gtk::Label 		*m_num_peers;

		Gtk::Label 		*m_num_complete_label;
		Gtk::Label 		*m_num_complete;

		Gtk::Label 		*m_num_incomplete_label;
		Gtk::Label 		*m_num_incomplete;

		Gtk::Label 		*m_list_seeds_label;
		Gtk::Label 		*m_list_seeds;

		Gtk::Label 		*m_list_peers_label;
		Gtk::Label 		*m_list_peers;

		Gtk::Label 		*m_connect_candidates_label;
	  Gtk::Label 		*m_connect_candidates;

		Gtk::Label 		*m_num_pieces_label;
		Gtk::Label 		*m_num_pieces;

		Gtk::Label 		*m_distributed_full_copies_label;
		Gtk::Label 		*m_distributed_full_copies;

		Gtk::Label 		*m_distributed_fraction_label;
		Gtk::Label 		*m_distributed_fraction;

		Gtk::Label 		*m_distributed_copies_label;
		Gtk::Label 		*m_distributed_copies;

		Gtk::Label 		*m_block_size_label;
	  Gtk::Label 		*m_block_size;

		Gtk::Label 		*m_num_uploads_label;
		Gtk::Label 		*m_num_uploads;

		Gtk::Label 		*m_num_connections_label;
		Gtk::Label 		*m_num_connections;

		Gtk::Label 		*m_uploads_limit_label;
		Gtk::Label 		*m_uploads_limit;

		Gtk::Label 		*m_connections_limit_label;
		Gtk::Label 		*m_connections_limit;

		Gtk::Label 		*m_up_bandwidth_queue_label;
		Gtk::Label 		*m_up_bandwidth_queue;

		Gtk::Label 		*m_down_bandwidth_queue_label;
		Gtk::Label 		*m_down_bandwidth_queue;

		Gtk::Label 		*m_time_since_upload_label;
		Gtk::Label 		*m_time_since_upload;

		Gtk::Label 		*m_time_since_download_label;
		Gtk::Label 		*m_time_since_download;

		Gtk::Label 		*m_active_time_label;
		Gtk::Label 		*m_active_time;

		Gtk::Label 		*m_finished_time_label;
		Gtk::Label 		*m_finished_time;

		Gtk::Label 		*m_seeding_time_label;
		Gtk::Label 		*m_seeding_time;

		Gtk::Label 		*m_seed_rank_label;
		Gtk::Label 		*m_seed_rank;

		Gtk::Label 		*m_last_scrape_label;
		Gtk::Label 		*m_last_scrape;

		Gtk::Label 		*m_sparse_regions_label;
		Gtk::Label 		*m_sparse_regions;

		Gtk::Label 		*m_priority_label;
	 	Gtk::Label 		*m_priority;

		Gtk::Label 		*m_state_label;
	  Gtk::Label 		*m_state;

		Gtk::Label 		*m_need_save_resume_label;
		Gtk::Label 		*m_need_save_resume;

		Gtk::Label 		*m_ip_filter_applies_label;
		Gtk::Label 		*m_ip_filter_applies;

		Gtk::Label 		*m_upload_mode_label;
		Gtk::Label 		*m_upload_mode;

		Gtk::Label 		*m_share_mode_label;
		Gtk::Label 		*m_share_mode;

		Gtk::Label 		*m_super_seeding_label;
	  Gtk::Label 		*m_super_seeding;

		Gtk::Label 		*m_paused_label;
	  Gtk::Label 		*m_paused;

		Gtk::Label 		*m_auto_managed_label;
		Gtk::Label 		*m_auto_managed;

		Gtk::Label 		*m_sequential_download_label;
		Gtk::Label 		*m_sequential_download;

		Gtk::Label 		*m_is_seeding_label;
		Gtk::Label 		*m_is_seeding;

		Gtk::Label 		*m_is_finished_label;
	  Gtk::Label 		*m_is_finished;

	 	Gtk::Label 		*m_has_metadata_label;
		Gtk::Label 		*m_has_metadata;

		Gtk::Label 		*m_has_incoming_label;
	  Gtk::Label 		*m_has_incoming;

		Gtk::Label 		*m_seed_mode_label;
	  Gtk::Label 		*m_seed_mode;

		Gtk::Label 		*m_moving_storage_label;
		Gtk::Label 		*m_moving_storage;

		Gtk::Label 		*m_info_hash_label;
		Gtk::Label 		*m_info_hash;

	public:

		GtkTorrentInfoBar();
		void updateInfo(shared_ptr<gt::Torrent> selected);
};
