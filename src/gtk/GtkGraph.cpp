#include <gtkmm/bin.h>
#include <gdkmm/general.h>
#include <gtkmm/menu.h>

#include <gtorrent/Settings.hpp>

#include "../Application.hpp"
#include "GtkGraph.hpp"

/**
 * Sets up the speed graph.
 */
GtkGraph::GtkGraph(GtkDrawingArea *da, const Glib::RefPtr<Gtk::Builder> rbuilder, unsigned size) :
	Gtk::Button(da),
	builder(rbuilder),
	m_maxSize(size)
{
	set_has_window(true);
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
 * Gets the vfunc() for speed graph.
 */
Gtk::SizeRequestMode GtkGraph::get_request_mode_vfunc() const
{
	//Accept the default value supplied by the base class.
	return Gtk::Widget::get_request_mode_vfunc();
}

/**
 * Discovers the total amount of minimum space and natural space needed by the speed graph widget.
 */
void GtkGraph::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	minimum_width = 100;
	natural_width = 500;
}

/**
 * Gets the preferred height for the speed graph widget for this particular width.
 */
void GtkGraph::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const
{
	get_preferred_height(minimum_height, natural_height);
}

/**
 * Gets the preferred height for the speed graph widget.
 */
void GtkGraph::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	minimum_height = 50;
	natural_height = 100;
}

/**
 * Gets the preferred width for the speed graph widget for this particular height.
 */
void GtkGraph::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const
{
	get_preferred_width(minimum_width, natural_width);
}

/**
 * Does something with the space that we have actually been given.
 */
void GtkGraph::on_size_allocate(Gtk::Allocation& allocation)
{
	//(We will not be given heights or widths less than we have requested, though
	//we might get more)

	//Use the offered allocation for this container:
	set_allocation(allocation);

	if(m_refGdkWindow)
	{
		m_refGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
		                             allocation.get_width(), allocation.get_height() );
	}
}

/**
 * Does something when the speed graph is mapped.
 */
void GtkGraph::on_map()
{
	//Call base class:
	Gtk::Widget::on_map();
}

/**
 * Does something when the speed graph is unmapped.
 */
void GtkGraph::on_unmap()
{
	//Call base class:
	Gtk::Widget::on_unmap();
}

/**
 * Does something when the speed graph is realized.
 */
void GtkGraph::on_realize()
{
	//Do not call base class Gtk::Widget::on_realize().
	//It's intended only for widgets that set_has_window(false).

	set_realized();

	if(!m_refGdkWindow)
	{
		//Create the GdkWindow:

		GdkWindowAttr attributes;
		memset(&attributes, 0, sizeof(attributes));

		Gtk::Allocation allocation = get_allocation();

		//Set initial position and size of the Gdk::Window:
		attributes.x = allocation.get_x();
		attributes.y = allocation.get_y();
		attributes.width = allocation.get_width();
		attributes.height = allocation.get_height();

		attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
		attributes.window_type = GDK_WINDOW_CHILD;
		attributes.wclass = GDK_INPUT_OUTPUT;

		m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes,
		                                     GDK_WA_X | GDK_WA_Y);
		set_window(m_refGdkWindow);

		//set colors TODO
		//override_background_color(Gdk::RGBA("red"));
		//override_color(Gdk::RGBA("green"));

		//make the widget receive expose events
		m_refGdkWindow->set_user_data(gobj());
	}
}

/**
 * Does something when the speed graph is unrealized.
 */
void GtkGraph::on_unrealize()
{
	m_refGdkWindow.reset();

	//Call base class:
	Gtk::Widget::on_unrealize();
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
		cr->line_to(i * ((width-m_labelLength)/5), 0);
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
		cr->move_to(0, 13 + (((height-m_labelHeight-13) / 5) * (i - 1)));
		cr->line_to(width - m_labelLength +2, 13 + (((height-m_labelHeight-13) / 5) * (i - 1)));
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
		Gtk::Menu   *m_rcMenu = Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem1 = Gtk::manage(new Gtk::MenuItem("1 hour"));
		Gtk::MenuItem *rcmItem2 = Gtk::manage(new Gtk::MenuItem("30 minutes"));
		Gtk::MenuItem *rcmItem3 = Gtk::manage(new Gtk::MenuItem("60 seconds"));


		rcmItem1->signal_activate().connect([this](){m_displaySize = 3600;});
		rcmItem2->signal_activate().connect([this](){m_displaySize = 1800;});
		rcmItem3->signal_activate().connect([this](){m_displaySize = 60;});

		m_rcMenu->add(*rcmItem1);
		m_rcMenu->add(*rcmItem2);
		m_rcMenu->add(*rcmItem3);

		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}
	return false;
}
/**
 * Resizes the history of the graph widget.
 */
void GtkGraph::resize(unsigned size)
{
	m_maxSize = size;
	for(auto &q : m_history)
	{
		while(q.second.first.size() > m_maxSize)
			q.second.first.pop();
		while(q.second.second.size() > m_maxSize)
			q.second.second.pop();
	}
}

/**
 * Adds upload and download values to the history of the torrent identified by the index
 */
void GtkGraph::add(std::shared_ptr<gt::Torrent> index, double upload, double download)
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
