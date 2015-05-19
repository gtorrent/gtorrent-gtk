#include "GtkRssBox.hpp"
#include "../../Application.hpp"

GtkRssBox::GtkRssBox(GtkBox *box, const Glib::RefPtr<Gtk::Builder> refBuilder)
: Gtk::Box(box),
  m_builder(refBuilder),
  m_core(Application::getSingleton()->getCore())
{
	Gtk::Label *label;

	show_all_children();
}

