#include "GtkAddRssWindow.hpp"

/**
* Displays a window for adding magnet links.
*/
GtkAddRssWindow::GtkAddRssWindow()
{
	this->set_modal(true);
	this->set_default_size(500, 250);

	this->set_title("Paste rss feed url here");

	m_textview = Gtk::manage(new Gtk::TextView());
	m_textview->set_wrap_mode(Gtk::WRAP_WORD_CHAR);

	this->get_vbox()->pack_start(*m_textview, true, true);

	this->add_button("Cancel", Gtk::RESPONSE_CANCEL);
	this->add_button("Add", Gtk::RESPONSE_OK);

	this->show_all();

	Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
	clip->request_text(sigc::mem_fun(*this, &GtkAddRssWindow::onClipboardReady));
}

/**
* Gets the magnet url.
*/
string GtkAddRssWindow::getRssURL()
{
	return m_textview->get_buffer()->get_text();
}

/**
* Gets text from the clipboard.
*/
void GtkAddRssWindow::onClipboardReady(const Glib::ustring &text)
{
	if (gt::Core::isMagnetLink(text))
	{
		Glib::RefPtr<Gtk::TextBuffer> buf = m_textview->get_buffer();
		buf->set_text(text);
	}
}

/**
* Adds the specified magnet link.
*/
void GtkAddRssWindow::onAddBtnClicked()
{
	close();
}