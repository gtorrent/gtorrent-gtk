#include "Base.hpp"
#include "../Application.hpp"
#include <gtorrent/Settings.hpp>

/**
* Sets a base for the rest of the application.
*/
GuiBase::GuiBase() :
	m_core(Application::getSingleton()->getCore())
{
}

void GuiBase::loadDefaultSettings()
{
	gt::Settings::settings["PausedForeGroundColor"]        = "#F08080";
	gt::Settings::settings["PausedBackGroundColor"]        = "#800000";
	gt::Settings::settings["QueuedForeGroundColor"]        = "#00BFFF";
	gt::Settings::settings["QueuedBackGroundColor"]        = "#FFFFFF";
	gt::Settings::settings["SeedingForeGroundColor"]       = "#1E90FF";
	gt::Settings::settings["SeedingBackGroundColor"]       = "#ADD8E6";
	gt::Settings::settings["MetadataForeGroundColor"]      = "#228B22";
	gt::Settings::settings["MetadataBackGroundColor"]      = "#7FFFD4";
	gt::Settings::settings["FinishedForeGroundColor"]      = "#ADD8E6";
	gt::Settings::settings["FinishedBackGroundColor"]      = "#483D8B";
	gt::Settings::settings["ResumingForeGroundColor"]      = "#6495ED";
	gt::Settings::settings["ResumingBackGroundColor"]      = "#FAF0E6";
	gt::Settings::settings["CheckingForeGroundColor"]      = "#DAA520";
	gt::Settings::settings["CheckingBackGroundColor"]      = "#FFFACD";
	gt::Settings::settings["AllocatingForeGroundColor"]    = "#FF7F50";
	gt::Settings::settings["AllocatingBackGroundColor"]    = "#FAFAD2";
	gt::Settings::settings["DownloadingForeGroundColor"]   = "#228B43";
	gt::Settings::settings["DownloadingBackGroundColor"]   = "#FFFFFF";
	gt::Settings::settings["CheckingQueueForeGroundColor"] = "#DAA520";
	gt::Settings::settings["CheckingQueueBackGroundColor"] = "#FFFACD";

	// Colors below can have an alpha channel
	gt::Settings::settings["GraphUploadCurveColor"  ] = "red";
	gt::Settings::settings["GraphDownloadCurveColor"] = "green";
	gt::Settings::settings["GraphUploadCurveStyle"  ] = "Line"; // Can also be Dash
	gt::Settings::settings["GraphDownloadCurveStyle"] = "Line";

	gt::Settings::settings["GraphStyle"] = "Curves"; // Can also be filled -> "Fill"	

	gt::Settings::settings["GraphUploadFillColor"  ] = "red";
	gt::Settings::settings["GraphDownloadFillColor"] = "green";

	gt::Settings::settings["GraphHLineColor"         ] = "grey";
	gt::Settings::settings["GraphVLineColor"         ] = "grey";
	gt::Settings::settings["GraphBorderColor"        ] = "grey";
	gt::Settings::settings["GraphGridColor"          ] = "grey";

	gt::Settings::settings["ShowLegend"             ] = "Yes";
	gt::Settings::settings["EnableNotification"     ] = "Yes";

}
