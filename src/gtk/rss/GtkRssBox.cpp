#include "GtkRssBox.hpp"
#include "../../Application.hpp"

GtkRssBox::GtkRssBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder)
: Gtk::Box(box),
  m_builder(refBuilder),
  m_core(Application::getSingleton()->getCore())
{
	m_builder->get_widget_derived("rss_treeview", m_rss_treeview);
	show_all_children();
}

