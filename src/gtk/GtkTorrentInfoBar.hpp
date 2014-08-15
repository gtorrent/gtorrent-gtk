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
public:
	GtkBlockBar *m_progress;
	Gtk::Label *m_title;
	Gtk::Label *m_transfer_label;
	Gtk::Label *m_general_label;
	GtkGraph *m_graph;
	Gtk::Notebook *m_notebook;
	Gtk::Box *m_general_box;
	Gtk::Box *m_trackers_box;
	Gtk::Box *m_peers_box;
	Gtk::Box *m_pieces_box;
	Gtk::Box *m_files_box;
	Gtk::Box *m_speed_box;
	Gtk::Box *m_log_box;

	Gtk::Box *m_piece_box;

	Gtk::Table *m_transfer_table_layout;

	Gtk::Label *m_time_elapsed_label;
	Gtk::Label *m_time_elapsed;
	Gtk::Label *m_down_total_label;
	Gtk::Label *m_down_total;
	Gtk::Label *m_down_speed_label;
	Gtk::Label *m_down_speed;
	Gtk::Label *m_down_limit_label;
	Gtk::Label *m_down_limit;
	Gtk::Label *m_status_label;
	Gtk::Label *m_status;

	Gtk::Label *m_remaining_label;
	Gtk::Label *m_remaining;
	Gtk::Label *m_up_total_label;
	Gtk::Label *m_up_total;
	Gtk::Label *m_up_speed_label;
	Gtk::Label *m_up_speed;
	Gtk::Label *m_up_limit_label;
	Gtk::Label *m_up_limit;

	Gtk::Label *m_wasted_label;
	Gtk::Label *m_wasted;
	Gtk::Label *m_seeds_label;
	Gtk::Label *m_seeds;
	Gtk::Label *m_leachers_label;
	Gtk::Label *m_leachers;
	Gtk::Label *m_share_ratio_label;
	Gtk::Label *m_share_ratio;

	Gtk::Table *m_general_table_layout;

	Gtk::Label *m_save_as_label;
	Gtk::Label *m_save_as;
	Gtk::Label *m_total_size_label;
	Gtk::Label *m_total_size;
	Gtk::Label *m_created_on_label;
	Gtk::Label *m_created_on;
	Gtk::Label *m_hash_label;
	Gtk::Label *m_hash;
	Gtk::Label *m_comment_label;
	Gtk::Label *m_comment;
	Gtk::Label *m_pieces_label;
	Gtk::Label *m_pieces;
	Gtk::Table *m_trackers_table_layout;

	Gtk::Label *m_name_label;
	Gtk::Label *m_name;

	Gtk::Label *m_tracker_name_label;
	Gtk::Label *m_tracker_status_label;
	Gtk::Label *m_tracker_update_in_label;
	Gtk::Label *m_tracker_seeds_label;
	Gtk::Label *m_tracker_peers_label;
	Gtk::Label *m_tracker_down_total_label;

	Gtk::Table *m_peers_table_layout;

	Gtk::Label *m_peers_ip_label;
	Gtk::Label *m_peers_client_label;
	Gtk::Label *m_peers_flags_label;
	Gtk::Label *m_peers_percent_label;
	Gtk::Label *m_peers_down_speed_label;
	Gtk::Label *m_peers_up_speed_label;
	Gtk::Label *m_peers_reqs_label;
	Gtk::Label *m_peers_uploaded_label;
	Gtk::Label *m_peers_downloaded_label;
	Gtk::Label *m_peers_peer_dl_label;

	Gtk::Table *m_pieces_table_layout;

	Gtk::Label *m_pieces_number_label;
	Gtk::Label *m_pieces_size_label;
	Gtk::Label *m_pieces_number_of_blocks_label;
	Gtk::Label *m_pieces_blocks_label;
	Gtk::Label *m_pieces_completed_label;
	Gtk::Label *m_pieces_availability_label;
	Gtk::Label *m_pieces_mode_label;

Gtk::Table *m_files_table_layout;

Gtk::Label *m_files_path_label;
Gtk::Label *m_files_size_label;
Gtk::Label *m_files_done_label;
Gtk::Label *m_files_percent_label;
Gtk::Label *m_files_number_of_pieces_label;
Gtk::Label *m_files_pieces_label;
Gtk::Label *m_files_priority_label;

Gtk::Label *m_log;
std::string getLog();
	GtkTorrentInfoBar();

	void updateInfo(std::shared_ptr<gt::Torrent> selected);
	void updateState(std::shared_ptr<gt::Torrent> selected);
};
