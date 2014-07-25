#include "GtkGraph.hpp"
#include <Application.hpp>
#include <gdkmm/general.h>  // for cairo helper functions
#include <gtk/gtkwidget.h>
#include <gtkmm/cssprovider.h>
#include <queue>
#include <cmath>

GtkGraph::GtkGraph(unsigned size) :
	//The GType name will actually be gtkmm__CustomObject_BlockBar
	Glib::ObjectBase("BlockBar"),
	Gtk::Widget(),
	m_maxSize(size)
{
	set_has_window(true);
}

GtkGraph::~GtkGraph()
{
}

Gtk::SizeRequestMode GtkGraph::get_request_mode_vfunc() const
{
	//Accept the default value supplied by the base class.
	return Gtk::Widget::get_request_mode_vfunc();
}

//Discover the total amount of minimum space and natural space needed by
//this widget.
void GtkGraph::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	minimum_width = 100;
	natural_width = 500;
}

void GtkGraph::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const
{
	get_preferred_height(minimum_height, natural_height);
}

void GtkGraph::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	minimum_height = 50;
	natural_height = 100;
}

void GtkGraph::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const
{
	get_preferred_width(minimum_width, natural_width);
}

void GtkGraph::on_size_allocate(Gtk::Allocation& allocation)
{
	//Do something with the space that we have actually been given:
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

void GtkGraph::on_map()
{
	//Call base class:
	Gtk::Widget::on_map();
}

void GtkGraph::on_unmap()
{
	//Call base class:
	Gtk::Widget::on_unmap();
}

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

void GtkGraph::on_unrealize()
{
	m_refGdkWindow.reset();

	//Call base class:
	Gtk::Widget::on_unrealize();
}

bool GtkGraph::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	const double width = (double)get_allocation().get_width();
	const double height = (double)get_allocation().get_height();

	Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());
	cr->fill();

	if(m_selected < m_history.size())
	{
		double increment = width / (double)(m_history[m_selected].first.size() - 1);
		unsigned order;
		if(max(m_history[m_selected].first, m_history[m_selected].second) <= 10 * 1024)
			order = 1;
		else if(max(m_history[m_selected].first, m_history[m_selected].second) <= 100 * 1024)
			order = 10;
		else if(max(m_history[m_selected].first, m_history[m_selected].second) <= 1000 * 1024)
			order = 100;
		else
			order = 1000;
		double maxValue = max(m_history[m_selected].first, m_history[m_selected].second) + (order * 1024) - (double)((int)(max(m_history[m_selected].first, m_history[m_selected].second)) % (order * 1024));

		// draw curves

		//Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("red"));
		ostringstream label;
		label << "Upload";
		cr->move_to(10, height / 2 - 15);
		cr->text_path(label.str());
		cr->fill();
		cr->stroke();
		draw(m_history[m_selected].first, height, increment, maxValue, cr);

		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("green"));
		label.str("");
		label.clear();
		label << "Download";
		cr->move_to(10, height / 2 - 30);
		cr->text_path(label.str());
		cr->fill();
		cr->stroke();
		draw(m_history[m_selected].second, height, increment, maxValue, cr);

		// draw grid
		Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("grey"));

		cr->move_to(0, 0);
		cr->line_to(width, 0);
		cr->stroke();
		label.str("");
		label.clear();
		if(order > 100)
			label << maxValue / (1024 * 1000) << "MiB/s";
		else
			label << maxValue / 1024 << "kiB/s";
		cr->move_to(0, 10);
		cr->text_path(label.str());
		cr->fill();
		cr->stroke();

		cr->move_to(0, height / 2);
		cr->line_to(width, height / 2);
		cr->stroke();
		label.str("");
		label.clear();
		if(order > 100)
			label << maxValue / (2 * 1024 * 1000) << "MiB/s";
		else
			label << maxValue / (2 * 1024) << "kiB/s";
		cr->move_to(0, height / 2 + 10);
		cr->text_path(label.str());
		cr->fill();
		cr->stroke();

		cr->move_to(0, height);
		cr->line_to(width, height);
		cr->stroke();
		label.str("");
		label.clear();
		if(order > 100)
			label << "0MiB/s";
		else
			label << "0kiB/s";
		cr->move_to(0, height - 5);
		cr->text_path(label.str());
		cr->fill();
		cr->stroke();

	}

	return true;
}

void GtkGraph::draw(queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr)
{
	if(!q.empty())
	{
		cr->move_to(0, height - (q.front() * height / maxValue));
		q.pop();

		double x = increment;
		while(!q.empty())
		{
			double y = height - (q.front() * height / maxValue);
			cr->line_to(x, y);
			//cout << x << ' ' << y << endl;
			q.pop();
			x += increment;
		}
		cr->stroke();
	}

}

void GtkGraph::resize(unsigned size)
{
	m_maxSize = size;
	for(auto &q : m_history)
	{
		while(q.first.size() > m_maxSize)
			q.first.pop();
		while(q.second.size() > m_maxSize)
			q.second.pop();
	}
}

/*void print(queue<double> q)// q passed by value on purpose*/
//{
//while(!q.empty())
//{
//cout << q.front() << ' ';
//q.pop();
//}
//cout << endl;
/*}*/

void GtkGraph::add(unsigned index, double upload, double download)
{
	// we want our vector to always have the same size as t
	vector<shared_ptr<gt::Torrent> > t = Application::getSingleton()->getCore()->getTorrents();
	m_history.resize(t.size());

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
	//print(m_history[index].second);
}

double GtkGraph::max(queue<double> q)// q passed by value on purpose
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

void GtkGraph::select(unsigned s)
{
	m_selected = s;
	queue_draw();
}
