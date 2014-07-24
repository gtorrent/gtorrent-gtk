#include "GtkGraph.hpp"
#include <gdkmm/general.h>  // for cairo helper functions
#include <gtk/gtkwidget.h>
#include <gtkmm/cssprovider.h>
#include <queue>

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
			
    Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());

	double increment = width / (double)m_history.size();
    double maxValue = max(m_history);

    queue<double> q = m_history;
    if(!q.empty())
    {
        cr->move_to(0, q.front() * height / maxValue);
        q.pop();

        double x = increment;
        while(!q.empty())
        {
            double y = q.front() * height / maxValue;
		    cr->line_to(x, y);
            q.pop();
            x += increment;
        }
    }
    cr->stroke();
	return true;
}

void GtkGraph::resize(unsigned size)
{
    m_maxSize = size;
    while(m_history.size() >= m_maxSize)
        m_history.pop();
}

void GtkGraph::add(double d)
{
    m_history.push(d);
    while(m_history.size() >= m_maxSize)
        m_history.pop();
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
