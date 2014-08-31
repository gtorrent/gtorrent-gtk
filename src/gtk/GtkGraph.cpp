#include <gtkmm/bin.h>
#include <gdkmm/general.h>
#include <gtkmm/menu.h>

#include <gtorrent/Settings.hpp>

#include "../Application.hpp"
#include "GtkGraph.hpp"

/**
 * Sets up the speed graph.
 */
GtkGraph::GtkGraph() :
	Gtk::Button()
{
	std::stringstream option(gt::Settings::settings["GraphIntervals"]);
	do
	{
		unsigned tmp;
		option >> tmp;
		if(tmp > m_maxSize)
			m_maxSize = tmp;
	}while(!option.eof());
	m_displaySize = std::stoul(gt::Settings::settings["GraphPreferredInterval"]);
}
bool upl = true;

std::string timestr(int64_t time_s)
{
	if(time_s < 0 )
		return "∞";
	else if(time_s == 0)
		return "0s";

	int64_t time_m = time_s / 60;
	time_s %= 60;
	int64_t time_h = time_m / 60;
	time_m %= 60;
	int64_t time_d = time_h / 24;
	time_h %= 24;

	std::ostringstream time_string;

	if ( time_d > 0 )
		time_string << time_d << "d ";
	if ( time_h > 0 )
		time_string << time_h << "h ";
	if ( time_m > 0 )
		time_string << time_m << "m ";
	if(time_s > 0)
		time_string << time_s << "s";

	return time_string.str();
}

std::string speedstr(int64_t file_size)
{
	std::ostringstream fss;

	if (file_size <= 0)
		return std::string();

	fss << std::setprecision(3);
	std::string units[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
	int e = ::floor(::log(file_size) / ::log(1024));
	fss << (file_size / ::pow(1024, e)) << " " << units[e];
	return fss.str() + "/s";
}

GtkGraph::~GtkGraph()
{
}

/**
 * Does something when the speed graph is drawn.
 */
bool GtkGraph::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->set_line_width(1.0);
	const double width = (double)get_allocation().get_width();
	const double height = (double)get_allocation().get_height();
	std::vector <double> dash = { 5 };
	Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());

	double increment = (width - m_labelLength) / (m_displaySize-1);
	std::queue<double> download = lastElements(m_history[m_selected].first, m_displaySize);
	std::queue<double> upload = lastElements(m_history[m_selected].second, m_displaySize);

	unsigned order;
	if(max(download, upload) <= 10 * 1024)
		order = 1;
	else if(max(download, upload) <= 100 * 1024)
		order = 10;
	else if(max(download, upload) <= 1000 * 1024)
		order = 100;
	else
		order = 1000;
	int maxValue = max(download, upload) + (order * 1024) - (double)((int)(max(m_history[m_selected].first, m_history[m_selected].second)) % (order * 1024));

	// draw curves

	//Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());
	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphUploadCurveColor"]));
	std::string label;
	if(gt::Settings::settings["ShowLegend"] != "No")
	{
		label = "Upload";
		cr->move_to(10, (height-m_labelHeight) / 2 - 15);
		cr->text_path(label);
		cr->fill();
	}
	upl = true;
	if(gt::Settings::settings["GraphUploadCurveStyle"] == "Dash")
		cr->set_dash(dash, 0);
	draw(download, (height-m_labelHeight), increment, maxValue, cr);
	cr->unset_dash();
	upl = false;
	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphDownloadCurveColor"]));

	if(gt::Settings::settings["ShowLegend"] != "No")
	{
		label = "Download";
		cr->move_to(10, (height-m_labelHeight) / 2 - 30);
		cr->text_path(label);
		cr->fill();
		cr->stroke();
	}

	if(gt::Settings::settings["GraphDownloadCurveStyle"] == "Dash")
		cr->set_dash(dash, 0);
	draw(upload, (height-m_labelHeight), increment, maxValue, cr);
	cr->unset_dash();

	// draw grid
	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphBorderColor"]));

	for(unsigned i = 0, val = 0;i<6;++i)
	{
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphBorderColor"]));
		cr->move_to(i * ((width-m_labelLength)/5), height);
		std::string label = timestr(val);
		cr->text_path(label);
		cr->fill();
		val += m_displaySize/5;

		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphHLineColor"]));
		cr->move_to(i * ((width-m_labelLength)/5), (height-m_labelHeight)+2);
		cr->line_to(i * ((width-m_labelLength)/5), gt::Settings::settings["ShowGrid"] == "Yes" ? 0 : (height-m_labelHeight));
		cr->stroke();
		cr->set_line_width(1.0);
	}
	cr->stroke();

	cr->move_to(0, (height-m_labelHeight));
	cr->line_to(width, (height-m_labelHeight));
	cr->move_to(0, (height-m_labelHeight));
	cr->line_to(0, 0);

	cr->move_to(width - m_labelLength, 0);
	cr->line_to(width - m_labelLength, (height-m_labelHeight));
	cr->stroke();

	int lValue = maxValue + 5 - (maxValue % 5);
	for(int i = 1; i <= 6; ++i)
	{
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphBorderColor"]));
		std::string label = speedstr(lValue - ((lValue / 5) * (i - 1)));
		cr->move_to(width - m_labelLength +2, 10 + (((height-m_labelHeight -10) / 5) * (i - 1)));
		cr->text_path(label);
		cr->fill();
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphHLineColor"]));
		cr->set_line_width(0.6);
		cr->move_to(width - m_labelLength +2, 13 + (((height-m_labelHeight-13) / 5) * (i - 1)));
		cr->line_to(gt::Settings::settings["ShowGrid"] == "Yes" ? 0 : width - m_labelLength, 13 + (((height-m_labelHeight-13) / 5) * (i - 1)));
		cr->stroke();
		cr->set_line_width(1.0);
	}

	Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings["GraphBorderColor"]));
	cr->move_to(width - 37, (height-m_labelHeight) - 5);
	cr->text_path("0B/s");
	cr->fill();

	return true;
}

/**
 * Draws a curve for the speed graph.
 */
void GtkGraph::draw(std::queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr)
{
	// wizards use computers
	// computers use numbers
	// no magic
	
	double offset = increment * (m_displaySize - q.size());

	cr->move_to(0, height);
	for(unsigned i = 0; i< (m_displaySize - q.size());++i)
		cr->line_to(i*increment, height);


	double oldy;
	if(q.empty()) return;

	oldy = height - (q.front() * height / maxValue);
	cr->line_to(offset, oldy);
	q.pop();
	double x = increment + offset;
	while(!q.empty())
	{
		double y = height - (q.front() * height / maxValue);
		cr->curve_to(x - increment/2, oldy, x - increment/2, y, x, y);
		q.pop();
		oldy = y;
		x += increment;
	}

	if(gt::Settings::settings["GraphStyle"] == "Fill")
	{
		cr->stroke_preserve();
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA(gt::Settings::settings[(upl) ? "GraphUploadFillColor" : "GraphDownloadFillColor"]));
		cr->line_to(x - increment, height);
		cr->line_to(0,height);
		auto k = Gdk::RGBA(gt::Settings::settings[(upl) ? "GraphUploadFillColor" : "GraphDownloadFillColor"]);
		cr->set_source_rgba(k.get_red(), k.get_green(), k.get_blue(), k.get_alpha() * 0.5);
		cr->fill();
	}
	else cr->stroke();
}

/**
 * Adds upload and download values to the history of the torrent identified by the index
 */
void GtkGraph::addValue(std::shared_ptr<gt::Torrent> index, double upload, double download)
{
	if(upload == NAN)
		upload = 0;
	if(download == NAN)
		download = 0;
	m_history[index].first.push(upload);
	while(m_history[index].first.size() > m_maxSize)
		m_history[index].first.pop();
	m_history[index].second.push(download);
	while(m_history[index].second.size() > m_maxSize)
		m_history[index].second.pop();
	queue_draw();
}

/**
 * Returns the maximum value of a queue or 0.
 */
double GtkGraph::max(std::queue<double> q)// q passed by value on purpose
{
	double max = 0;
	while(!q.empty())
	{
		if(q.front() > max)
			max = q.front();
		q.pop();
	}
	return max;
}

/**
 * Selects by index the torrent history to be displayed.
*/
void GtkGraph::select(std::shared_ptr<gt::Torrent> s)
{
	m_selected = s;
	queue_draw();
}

std::queue<double> GtkGraph::lastElements(std::queue<double> q, unsigned n)
{
	while(q.size() > n)
		q.pop();
	return q;
}

bool GtkGraph::on_button_press_event(GdkEventButton *event)
{
	if(event->button == 3) // if right-click
	{

		std::stringstream option(gt::Settings::settings["GraphIntervals"]);
		std::vector<unsigned> intervals;
		do
		{
			unsigned tmp;
			option >> tmp;
			intervals.push_back(tmp);
		}while(!option.eof());

		Gtk::Menu   *rcMenu = Gtk::manage(new Gtk::Menu());
		std::vector<Gtk::MenuItem*> rcItems;
		rcItems.resize(intervals.size());
		for(unsigned i = 0;i<intervals.size();++i)
		{
			rcItems[i] = Gtk::manage(new Gtk::MenuItem(timestr(intervals[i])));
			rcItems[i]->signal_activate().connect([this,i,intervals](){m_displaySize = intervals[i];});
			rcMenu->add(*(rcItems[i]));
		}

		rcMenu->show_all();
		rcMenu->popup(event->button, event->time);
	}
	return false;
}

