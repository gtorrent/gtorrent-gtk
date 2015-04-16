#include "GtkTorrentBox.hpp"
#include "../../Application.hpp"

#include <gtkmm/builder.h>

GtkTorrentBox::GtkTorrentBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder>& refBuilder) :
	Gtk::Box(box),
	builder(refBuilder),
	m_core(Application::getSingleton()->getCore())
{
	builder->get_widget("torrentbox", m_torrentbox);
	builder->get_widget("searchbar", m_searchbar);
	builder->get_widget("panel", m_panel);
	builder->get_widget("stack_main", m_stack_main);
	builder->get_widget("stack_info", m_stack_info);
}
