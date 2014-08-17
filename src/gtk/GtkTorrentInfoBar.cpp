#include "GtkTorrentInfoBar.hpp"

#include <gtkmm/table.h>
#include <gtkmm/hvseparator.h>

/**
* Sets up the torrent info bar.
*/
GtkTorrentInfoBar::GtkTorrentInfoBar()
	: Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
	//TODO: better layout
	m_notebook = Gtk::manage(new Gtk::Notebook());
	m_general_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_trackers_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_peers_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_pieces_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_files_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_speed_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_log_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	m_piece_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	m_title = Gtk::manage(new Gtk::Label());

	this->pack_start(*m_title, Gtk::PACK_SHRINK);

	m_progress = Gtk::manage(new GtkBlockBar());
	m_graph = Gtk::manage(new GtkGraph());

	m_piece_box->pack_end(*m_progress, Gtk::PACK_EXPAND_WIDGET, 0);
	m_progress->set_size_request(0, 50);

	m_pieces_box->add(*m_piece_box);
	m_speed_box->add(*m_graph);

	m_transfer_label = Gtk::manage(new Gtk::Label());
	m_transfer_label->set_markup("<b>Transfer</b>");

	m_general_box->add(*m_transfer_label);

	m_transfer_table_layout = Gtk::manage(new Gtk::Table(12, 5, false));
	m_transfer_table_layout->set_col_spacings(5);

	m_time_elapsed_label = Gtk::manage(new Gtk::Label());
	m_time_elapsed_label->set_markup("<b>Time Elapsed</b>");
	m_time_elapsed = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_time_elapsed_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_time_elapsed, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);

	m_down_total_label = Gtk::manage(new Gtk::Label());
	m_down_total_label->set_markup("<b>Downloaded</b>");
	m_down_total = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_down_total_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_down_total, 2, 3, 1, 2, Gtk::AttachOptions::SHRINK);

	m_down_speed_label = Gtk::manage(new Gtk::Label());
	m_down_speed_label->set_markup("<b>Download Speed</b>");
	m_down_speed = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_down_speed_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_down_speed, 2, 3, 2, 3, Gtk::AttachOptions::SHRINK);
/*
	m_down_limit_label = Gtk::manage(new Gtk::Label());
	m_down_limit_label->set_markup("<b>Down Limit</b>");
	m_down_limit = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_down_limit_label, 0, 1, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_down_limit, 2, 3, 3, 4, Gtk::AttachOptions::SHRINK);
*/
	m_status_label = Gtk::manage(new Gtk::Label());
	m_status_label->set_markup("<b>Status</b>");
	m_status = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_status_label, 0, 1, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_status, 2, 3, 3, 4, Gtk::AttachOptions::SHRINK);


	m_remaining_label = Gtk::manage(new Gtk::Label());
	m_remaining_label->set_markup("<b>Remaining</b>");
	m_remaining = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_remaining_label, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_remaining, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);

	m_up_total_label = Gtk::manage(new Gtk::Label());
	m_up_total_label->set_markup("<b>Uploaded</b>");
	m_up_total = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_up_total_label, 4, 5, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_up_total, 6, 7, 1, 2, Gtk::AttachOptions::SHRINK);

	m_up_speed_label = Gtk::manage(new Gtk::Label());
	m_up_speed_label->set_markup("<b>Upload Speed</b>");
	m_up_speed = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_up_speed_label, 4, 5, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_up_speed, 6, 7, 2, 3, Gtk::AttachOptions::SHRINK);
/*
	m_up_limit_label = Gtk::manage(new Gtk::Label());
	m_up_limit_label->set_markup("<b>Up Limit</b>");
	m_up_limit = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_up_limit_label, 4, 5, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_up_limit, 7, 8, 3, 4, Gtk::AttachOptions::SHRINK);


	m_wasted_label = Gtk::manage(new Gtk::Label());
	m_wasted_label->set_markup("<b>Wasted</b>");
	m_wasted = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_wasted_label, 9, 10, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 10, 11, 0, 1, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_wasted, 11, 12, 0, 1, Gtk::AttachOptions::SHRINK);
*/
	m_seeds_label = Gtk::manage(new Gtk::Label());
	m_seeds_label->set_markup("<b>Seeds</b>");
	m_seeds = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_seeds_label, 9, 10, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 10, 11, 1, 2, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_seeds, 11, 12, 1, 2, Gtk::AttachOptions::SHRINK);

	m_peers_label = Gtk::manage(new Gtk::Label());
	m_peers_label->set_markup("<b>Peers</b>");
	m_peers = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_peers_label, 9, 10, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 10, 11, 2, 3, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_peers, 11, 12, 2, 3, Gtk::AttachOptions::SHRINK);

	m_share_ratio_label = Gtk::manage(new Gtk::Label());
	m_share_ratio_label->set_markup("<b>Share Ratio</b>");
	m_share_ratio = Gtk::manage(new Gtk::Label("0"));
	m_transfer_table_layout->attach(*m_share_ratio_label, 9, 10, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*(new Gtk::VSeparator()), 10, 11, 3, 4, Gtk::AttachOptions::SHRINK);
	m_transfer_table_layout->attach(*m_share_ratio, 11, 12, 3, 4, Gtk::AttachOptions::SHRINK);

	m_general_box->pack_start(*(new Gtk::HSeparator()), Gtk::PACK_SHRINK);
	m_general_box->pack_start(*m_transfer_table_layout, Gtk::PACK_SHRINK);

	m_general_label = Gtk::manage(new Gtk::Label());
	m_general_label->set_markup("<b>General</b>");

	m_general_box->add(*m_general_label);

	m_general_table_layout = Gtk::manage(new Gtk::Table(7, 5, false));
	m_general_table_layout->set_col_spacings(5);

	m_save_as_label = Gtk::manage(new Gtk::Label());
	m_save_as_label->set_markup("<b>Save As</b>");
	m_save_as = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_save_as_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_save_as, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);

	m_total_size_label = Gtk::manage(new Gtk::Label());
	m_total_size_label->set_markup("<b>Size</b>");
	m_total_size = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_total_size_label, 0, 1, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_total_size, 2, 3, 1, 2, Gtk::AttachOptions::SHRINK);

	m_total_downloaded_label = Gtk::manage(new Gtk::Label());
	m_total_downloaded_label->set_markup("<b>Downloaded</b>");
	m_total_downloaded = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_total_downloaded_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_total_downloaded, 2, 3, 2, 3, Gtk::AttachOptions::SHRINK);
/*
	m_created_on_label = Gtk::manage(new Gtk::Label());
	m_created_on_label->set_markup("<b>Created On</b>");
	m_created_on = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_created_on_label, 0, 1, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_created_on, 2, 3, 2, 3, Gtk::AttachOptions::SHRINK);
*/
	m_hash_label = Gtk::manage(new Gtk::Label());
	m_hash_label->set_markup("<b>Hash</b>");
	m_hash = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_hash_label, 0, 1, 3, 4, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 3, 4, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_hash, 2, 3, 3, 4, Gtk::AttachOptions::SHRINK);
/*
	m_comment_label = Gtk::manage(new Gtk::Label());
	m_comment_label->set_markup("<b>Comment</b>");
	m_comment = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_comment_label, 0, 1, 4, 5, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 4, 5, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_comment, 2, 3, 4, 5, Gtk::AttachOptions::SHRINK);

	m_pieces_label = Gtk::manage(new Gtk::Label());
	m_pieces_label->set_markup("<b>Total Pieces</b>");
	m_pieces = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_pieces_label, 4, 5, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 1, 2, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_pieces, 6, 7, 1, 2, Gtk::AttachOptions::SHRINK);
*/
	m_downloaded_pieces_label = Gtk::manage(new Gtk::Label());
	m_downloaded_pieces_label->set_markup("<b>Downloaded Pieces</b>");
	m_downloaded_pieces = Gtk::manage(new Gtk::Label("0"));
	m_general_table_layout->attach(*m_downloaded_pieces_label, 4, 5, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 2, 3, Gtk::AttachOptions::SHRINK);
	m_general_table_layout->attach(*m_downloaded_pieces, 6, 7, 2, 3, Gtk::AttachOptions::SHRINK);

	m_general_box->add(*m_general_table_layout);

	m_trackers_table_layout = Gtk::manage(new Gtk::Table(12, 1, false));
	m_trackers_table_layout->set_col_spacings(5);

	m_tracker_name_label = Gtk::manage(new Gtk::Label());
	m_tracker_name_label->set_markup("<b>Name</b>");
	m_trackers_table_layout->attach(*m_tracker_name_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

	m_tracker_status_label = Gtk::manage(new Gtk::Label());
	m_tracker_status_label->set_markup("<b>Status</b>");
	m_trackers_table_layout->attach(*m_tracker_status_label, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);

	m_tracker_update_in_label = Gtk::manage(new Gtk::Label());
	m_tracker_update_in_label->set_markup("<b>Update In</b>");
	m_trackers_table_layout->attach(*m_tracker_update_in_label, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::SHRINK);

	m_tracker_seeds_label = Gtk::manage(new Gtk::Label());
	m_tracker_seeds_label->set_markup("<b>Seeds</b>");
	m_trackers_table_layout->attach(*m_tracker_seeds_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);

	m_tracker_peers_label = Gtk::manage(new Gtk::Label());
	m_tracker_peers_label->set_markup("<b>Peers</b>");
	m_trackers_table_layout->attach(*m_tracker_peers_label, 8, 9, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 9, 10, 0, 1, Gtk::AttachOptions::SHRINK);

	m_tracker_down_total_label = Gtk::manage(new Gtk::Label());
	m_tracker_down_total_label->set_markup("<b>Downloaded</b>");
	m_trackers_table_layout->attach(*m_tracker_down_total_label, 10, 11, 0, 1, Gtk::AttachOptions::SHRINK);
	m_trackers_table_layout->attach(*(new Gtk::VSeparator()), 11, 12, 0, 1, Gtk::AttachOptions::SHRINK);

	m_trackers_box->add(*m_trackers_table_layout);

	m_peers_table_layout = Gtk::manage(new Gtk::Table(20, 1, false));
	m_peers_table_layout->set_col_spacings(5);

	m_peers_ip_label = Gtk::manage(new Gtk::Label());
	m_peers_ip_label->set_markup("<b>IP</b>");
	m_peers_table_layout->attach(*m_peers_ip_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_client_label = Gtk::manage(new Gtk::Label());
	m_peers_client_label->set_markup("<b>Client</b>");
	m_peers_table_layout->attach(*m_peers_client_label, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_flags_label = Gtk::manage(new Gtk::Label());
	m_peers_flags_label->set_markup("<b>Flags</b>");
	m_peers_table_layout->attach(*m_peers_flags_label, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_percent_label = Gtk::manage(new Gtk::Label());
	m_peers_percent_label->set_markup("<b>Percent</b>");
	m_peers_table_layout->attach(*m_peers_percent_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_down_speed_label = Gtk::manage(new Gtk::Label());
	m_peers_down_speed_label->set_markup("<b>Down Speed</b>");
	m_peers_table_layout->attach(*m_peers_down_speed_label, 8, 9, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 9, 10, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_up_speed_label = Gtk::manage(new Gtk::Label());
	m_peers_up_speed_label->set_markup("<b>Up Speed</b>");
	m_peers_table_layout->attach(*m_peers_up_speed_label, 10, 11, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 11, 12, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_reqs_label = Gtk::manage(new Gtk::Label());
	m_peers_reqs_label->set_markup("<b>Reqs</b>");
	m_peers_table_layout->attach(*m_peers_reqs_label, 12, 13, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 13, 14, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_uploaded_label = Gtk::manage(new Gtk::Label());
	m_peers_uploaded_label->set_markup("<b>Uploaded</b>");
	m_peers_table_layout->attach(*m_peers_uploaded_label, 14, 15, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 15, 16, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_downloaded_label = Gtk::manage(new Gtk::Label());
	m_peers_downloaded_label->set_markup("<b>Downloaded</b>");
	m_peers_table_layout->attach(*m_peers_downloaded_label, 16, 17, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 17, 18, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_peer_dl_label = Gtk::manage(new Gtk::Label());
	m_peers_peer_dl_label->set_markup("<b>Peer Download</b>");
	m_peers_table_layout->attach(*m_peers_peer_dl_label, 18, 19, 0, 1, Gtk::AttachOptions::SHRINK);
	m_peers_table_layout->attach(*(new Gtk::VSeparator()), 19, 20, 0, 1, Gtk::AttachOptions::SHRINK);

	m_peers_box->add(*m_peers_table_layout);

	m_pieces_table_layout = Gtk::manage(new Gtk::Table(14, 1, false));
	m_pieces_table_layout->set_col_spacings(5);

	m_pieces_number_label = Gtk::manage(new Gtk::Label());
	m_pieces_number_label->set_markup("<b>Number</b>");
	m_pieces_table_layout->attach(*m_pieces_number_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_size_label = Gtk::manage(new Gtk::Label());
	m_pieces_size_label->set_markup("<b>Size</b>");
	m_pieces_table_layout->attach(*m_pieces_size_label, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_number_of_blocks_label = Gtk::manage(new Gtk::Label());
	m_pieces_number_of_blocks_label->set_markup("<b>Number Of Blocks</b>");
	m_pieces_table_layout->attach(*m_pieces_number_of_blocks_label, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_blocks_label = Gtk::manage(new Gtk::Label());
	m_pieces_blocks_label->set_markup("<b>Blocks</b>");
	m_pieces_table_layout->attach(*m_pieces_blocks_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_completed_label = Gtk::manage(new Gtk::Label());
	m_pieces_completed_label->set_markup("<b>Completed</b>");
	m_pieces_table_layout->attach(*m_pieces_completed_label, 8, 9, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 9, 10, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_availability_label = Gtk::manage(new Gtk::Label());
	m_pieces_availability_label->set_markup("<b>Availability</b>");
	m_pieces_table_layout->attach(*m_pieces_availability_label, 10, 11, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 11, 12, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_mode_label = Gtk::manage(new Gtk::Label());
	m_pieces_mode_label->set_markup("<b>Mode</b>");
	m_pieces_table_layout->attach(*m_pieces_mode_label, 12, 13, 0, 1, Gtk::AttachOptions::SHRINK);
	m_pieces_table_layout->attach(*(new Gtk::VSeparator()), 13, 14, 0, 1, Gtk::AttachOptions::SHRINK);

	m_pieces_box->add(*m_pieces_table_layout);

	m_files_table_layout = Gtk::manage(new Gtk::Table(14, 1, false));
	m_files_table_layout->set_col_spacings(5);

	m_files_path_label = Gtk::manage(new Gtk::Label());
	m_files_path_label->set_markup("<b>Path</b>");
	m_files_table_layout->attach(*m_files_path_label, 0, 1, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 1, 2, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_size_label = Gtk::manage(new Gtk::Label());
	m_files_size_label->set_markup("<b>Size</b>");
	m_files_table_layout->attach(*m_files_size_label, 2, 3, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 3, 4, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_done_label = Gtk::manage(new Gtk::Label());
	m_files_done_label->set_markup("<b>Done</b>");
	m_files_table_layout->attach(*m_files_done_label, 4, 5, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 5, 6, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_percent_label = Gtk::manage(new Gtk::Label());
	m_files_percent_label->set_markup("<b>Percent</b>");
	m_files_table_layout->attach(*m_files_percent_label, 6, 7, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 7, 8, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_number_of_pieces_label = Gtk::manage(new Gtk::Label());
	m_files_number_of_pieces_label->set_markup("<b>Number Of Pieces</b>");
	m_files_table_layout->attach(*m_files_number_of_pieces_label, 8, 9, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 9, 10, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_pieces_label = Gtk::manage(new Gtk::Label());
	m_files_pieces_label->set_markup("<b>Pieces</b>");
	m_files_table_layout->attach(*m_files_pieces_label, 10, 11, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 11, 12, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_priority_label = Gtk::manage(new Gtk::Label());
	m_files_priority_label->set_markup("<b>Priority</b>");
	m_files_table_layout->attach(*m_files_priority_label, 12, 13, 0, 1, Gtk::AttachOptions::SHRINK);
	m_files_table_layout->attach(*(new Gtk::VSeparator()), 13, 14, 0, 1, Gtk::AttachOptions::SHRINK);

	m_files_box->add(*m_files_table_layout);

m_log = Gtk::manage(new Gtk::Label("0"));
m_log_box->add(*m_log);
	m_notebook->append_page(*m_general_box, "General");
	m_notebook->append_page(*m_trackers_box, "Trackers");
	m_notebook->append_page(*m_peers_box, "Peers");
	m_notebook->append_page(*m_pieces_box, "Pieces");
	m_notebook->append_page(*m_files_box, "Files");
	m_notebook->append_page(*m_speed_box, "Speed");
	m_notebook->append_page(*m_log_box, "Log");
	this->pack_end(*m_notebook, Gtk::PACK_EXPAND_WIDGET, 5);
}

// TODO: Should replace every place where a torrent index is required with a torrent pointer, smells like everything would break if
// the user tried to sort the torrents.

/**
* Updates the torrent info bar.
*/
void GtkTorrentInfoBar::updateInfo(std::shared_ptr<gt::Torrent> selected)
{
	static std::shared_ptr<gt::Torrent> previous = nullptr;
	int selectedIndex = 0;
	std::vector<std::shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();

	if(selected)
	{
		//set_visible(true);
	}
	else
	{
		//set_visible(false);
		return;
	}

	for(unsigned i = 0; i < t.size(); ++i)
		if(selected == t[i])
			selectedIndex = i;

	if(t[selectedIndex]->getHandle().status().has_metadata) // torrentless torrents (magnet links) can't have pieces
		m_progress->setBlocks(t[selectedIndex]->getPieces());

	m_title->set_text(t[selectedIndex]->getName());
	m_graph->select(selectedIndex);

	if(previous != selected)
	{
		m_time_elapsed->set_text(t[selectedIndex]->getTextActiveTime());
		m_down_total->set_text(t[selectedIndex]->getTextTotalDownloaded());
		m_down_speed->set_text(t[selectedIndex]->getTextDownloadRate());
		//m_down_limit->set_text(t[selectedIndex]->getTextActiveTime());
		m_status->set_text(t[selectedIndex]->getTextState());

		m_remaining->set_text(t[selectedIndex]->getTextEta());
		m_up_total->set_text(t[selectedIndex]->getTextTotalUploaded());
		m_up_speed->set_text(t[selectedIndex]->getTextUploadRate());
		//m_up_limit->set_text(t[selectedIndex]->getTextActiveTime());

		//m_wasted->set_text(t[selectedIndex]->getTextActiveTime());
		m_seeds->set_text(t[selectedIndex]->getTextTotalSeeders());
		m_peers->set_text(t[selectedIndex]->getTextTotalPeers());
		m_share_ratio->set_text(t[selectedIndex]->getTextTotalRatio());

		m_save_as->set_text(t[selectedIndex]->getSavePath());
m_total_size->set_text(t[selectedIndex]->getTextSize());
m_hash->set_text(t[selectedIndex]->getTextHash());
m_total_downloaded->set_text(t[selectedIndex]->getTextTotalDownloaded());

m_downloaded_pieces->set_text(t[selectedIndex]->getTextDownloadedPieces());

		m_log->set_text(getLog());

	}
	previous = selected;
}

void GtkTorrentInfoBar::updateState(std::shared_ptr<gt::Torrent> selected)
{
	if(!selected) return updateInfo(selected);
	int selectedIndex = 0;
	std::vector<std::shared_ptr<gt::Torrent>> t = Application::getSingleton()->getCore()->getTorrents();
	for(unsigned i = 0; i < t.size(); ++i)
		if(selected == t[i])
			selectedIndex = i;
	if(t[selectedIndex]->getHandle().status().has_metadata)
		m_progress->setBlocks(t[selectedIndex]->getPieces());
	m_time_elapsed->set_text(t[selectedIndex]->getTextActiveTime());
	m_down_total->set_text(t[selectedIndex]->getTextTotalDownloaded());
	m_down_speed->set_text(t[selectedIndex]->getTextDownloadRate());
	//m_down_limit->set_text(downlimit);
	m_status->set_text(t[selectedIndex]->getTextState());

	m_remaining->set_text(t[selectedIndex]->getTextEta());
	m_up_total->set_text(t[selectedIndex]->getTextTotalUploaded());
	m_up_speed->set_text(t[selectedIndex]->getTextUploadRate());
	//m_up_limit->set_text(uplimit);

	//m_wasted->set_text(t[selectedIndex]->getTextActiveTime());
	m_seeds->set_text(t[selectedIndex]->getTextTotalSeeders());
	m_peers->set_text(t[selectedIndex]->getTextTotalPeers());
	m_share_ratio->set_text(t[selectedIndex]->getTextTotalRatio());

m_save_as->set_text(t[selectedIndex]->getSavePath());
m_total_size->set_text(t[selectedIndex]->getTextSize());
m_total_downloaded->set_text(t[selectedIndex]->getTextTotalDownloaded());

//m_pieces->set_text(t[selectedIndex]->getTextPieces());
m_downloaded_pieces->set_text(t[selectedIndex]->getTextDownloadedPieces());

m_log->set_text(getLog());

	for(unsigned i = 0; i < t.size(); ++i)
		m_graph->add(i, (double)t[i]->getUploadRate(), (double)t[i]->getDownloadRate());

}

std::string GtkTorrentInfoBar::getLog()
{
	// TODO: Find a less harmful way to do logging
	/*std::ifstream t("gtorrent.log");
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();*/
	return "TODO";
}
