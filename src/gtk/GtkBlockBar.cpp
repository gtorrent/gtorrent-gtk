#include "GtkBlockBar.hpp"

#include <vector>

/**
* Sets up the block bar.
*/
GtkBlockBar::GtkBlockBar() :
	//The GType name will actually be gtkmm__CustomObject_BlockBar
	Glib::ObjectBase("BlockBar"),
	Gtk::Widget()
{
	set_has_window(true);
}

GtkBlockBar::~GtkBlockBar()
{
}

/**
* Gets the vfunc() for block bar.
*/
Gtk::SizeRequestMode GtkBlockBar::get_request_mode_vfunc() const
{
	//Accept the default value supplied by the base class.
	return Gtk::Widget::get_request_mode_vfunc();
}

/**
* Discovers the total amount of minimum space and natural space needed by the block bar widget.
*/
void GtkBlockBar::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
	minimum_width = 760;
	natural_width = 1024;
}

/**
* Gets the preferred height for the block bar widget for this particular width.
*/
void GtkBlockBar::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const
{
	get_preferred_height(minimum_height, natural_height);
}

/**
* Gets the preferred height for the block bar widget.
*/
void GtkBlockBar::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
	minimum_height = 22;
	natural_height = 22;
}

/**
* Gets the preferred width for the block bar widget for this particular height.
*/
void GtkBlockBar::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const
{
	get_preferred_width(minimum_width, natural_width);
}

/**
* Does something with the space that we have actually been given.
*/
void GtkBlockBar::on_size_allocate(Gtk::Allocation& allocation)
{
	//
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
* Does something when the block bar is mapped.
*/
void GtkBlockBar::on_map()
{
	//Call base class:
	Gtk::Widget::on_map();
}

/**
* Does something when the block bar is unmapped.
*/
void GtkBlockBar::on_unmap()
{
	//Call base class:
	Gtk::Widget::on_unmap();
}

/**
* Does something when the block bar is realized.
*/
void GtkBlockBar::on_realize()
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
* Does something when the block bar is unrealized.
*/
void GtkBlockBar::on_unrealize()
{
	m_refGdkWindow.reset();

	//Call base class:
	Gtk::Widget::on_unrealize();
}

/**
* Does something when the block bar is drawn.
*/
bool GtkBlockBar::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	const double width = (double)get_allocation().get_width();
	const double height = (double)get_allocation().get_height();

	double increment = width / (double)m_blocks.size();
	for(unsigned i = 0; i < m_blocks.size(); ++i)
	{
		if(m_blocks[i])
			Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_color());
		else
			Gdk::Cairo::set_source_rgba(cr, get_style_context()->get_background_color());

		double start = i * increment;
		cr->rectangle(start, 0, increment, height);
		cr->stroke_preserve();
		cr->fill();
	}
	return true;
}

/**
* Sets the blocks in block bar.
*/
void GtkBlockBar::setBlocks(std::vector<bool> b)
{
	m_blocks = b;
	queue_draw();
}
