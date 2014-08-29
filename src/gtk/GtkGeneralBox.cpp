#include <gtkmm/hvseparator.h>
#include <gtorrent/Torrent.hpp>

#include "GtkGeneralBox.hpp"

// TODO: remove all this boilerplate and replace it with initialize methods instead of doing the same thing a dozen times

GtkGeneralBox::GtkGeneralBox(GtkGrid *grid, const Glib::RefPtr<Gtk::Builder> rbuilder) : Gtk::Grid(grid), builder(rbuilder)
{
	rbuilder->get_widget("DownSpeed"    , m_download_speed);
	rbuilder->get_widget("Hash"         , m_hash          );
	rbuilder->get_widget("Pieces"       , m_pieces        );
	rbuilder->get_widget("Remaining"    , m_remaining     );
	rbuilder->get_widget("SavePath"     , m_save_as       );
	rbuilder->get_widget("ShareRatio"   , m_share_ratio   );
	rbuilder->get_widget("TimeElapsed"  , m_time_elapsed  );
	rbuilder->get_widget("TotalSize"    , m_total_size    );
	rbuilder->get_widget("TrackerURL"   , m_tracker_url   );
	rbuilder->get_widget("UploadSpeed"  , m_upload_speed  );
	rbuilder->get_widget("Uploaded"     , m_uploaded      );
	rbuilder->get_widget("Downloaded"   , m_downloaded    );
	rbuilder->get_widget("DownLimit"    , m_down_limit    );
	rbuilder->get_widget("UpLimit"      , m_up_limit      );
	rbuilder->get_widget("TrackerStatus", m_tracker_status);
}

void GtkGeneralBox::update(std::shared_ptr<gt::Torrent> selected)
{
	m_download_speed->set_text(selected->getTextDownloadRate());
	m_hash          ->set_text(selected->getFormattedHash());
	m_pieces        ->set_text(std::to_string(selected->getPieces().size()));
	m_remaining     ->set_text(selected->getTextEta());
	m_save_as       ->set_text(selected->getSavePath());
	m_share_ratio   ->set_text(std::to_string(selected->getTotalRatio()));
	m_time_elapsed  ->set_text(selected->getTextActiveTime());
	m_total_size    ->set_text(selected->getTextSize());
	m_tracker_url   ->set_text(selected->getCurrentTrackerURL());
	m_upload_speed  ->set_text(selected->getTextUploadRate());
	m_uploaded      ->set_text(selected->getTextTotalUploaded());
	m_downloaded    ->set_text(selected->getTextTotalDownloaded());
	m_down_limit    ->set_text("");
	m_up_limit      ->set_text("");
	m_tracker_status->set_text("");
}
