#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/stock.h>
#include <glibmm.h>

GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(800, 500);
	m_treeview = Gtk::manage(new GtkTorrentTreeView());
	this->add(*m_treeview);
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 10);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);

	//Gtk::Button *btn_connect     = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_add_torrent = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_add_link    = Gtk::manage(new Gtk::Button());
	Gtk::VSeparator *separator  = Gtk::manage(new Gtk::VSeparator());
	Gtk::Button *btn_up          = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_down        = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_resume      = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_pause       = Gtk::manage(new Gtk::Button());
	Gtk::Button *btn_remove      = Gtk::manage(new Gtk::Button());

	btn_add_link->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	btn_add_torrent->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	btn_pause->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	btn_resume->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));

	//btn_connect->set_image_from_icon_name     ("gtk-directory");
	btn_add_torrent->set_image_from_icon_name ("gtk-add");
	btn_add_link->set_image_from_icon_name    ("edit-paste");
	btn_up->set_image_from_icon_name          ("gtk-go-up");
	btn_down->set_image_from_icon_name        ("gtk-go-down");
	btn_resume->set_image_from_icon_name      ("media-playback-start");
	btn_pause->set_image_from_icon_name       ("media-playback-pause");
	btn_remove->set_image_from_icon_name      ("gtk-cancel");

	//header->add(*btn_connect);
	header->add(*btn_add_torrent);
	header->add(*btn_add_link);
	header->add(*btn_up);
	header->add(*btn_down);
	header->add(*btn_resume);
	header->add(*btn_pause);
	header->add(*separator);
	header->add(*btn_remove);

	this->set_titlebar(*header);
	this->show_all();
}

bool GtkMainWindow::onSecTick()
{
	m_treeview->updateCells();
	return true;
}

void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse for torrent file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_default_size(256, 256);
	//fc.set_window_position(0, 128);
	fc.set_select_multiple();
	fc.set_transient_for(*this);
	fc.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	fc.add_button("Select", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> filter_t = Gtk::FileFilter::create();
	filter_t->set_name("Torrent Files");
	filter_t->add_mime_type("application/x-bittorrent");
	fc.add_filter(filter_t);

	int result = fc.run();

	switch (result)
	{
	case Gtk::RESPONSE_OK:
		for (auto & f : fc.get_filenames())
		{
			shared_ptr<Torrent> t = m_core->addTorrent(f.c_str());
			if (t)//Checks if t is not null
				m_treeview->addCell(t);
			//TODO Add error dialogue if torrent add is unsuccessful
		}
		break;
	}
}

void GtkMainWindow::onAddMagnetBtnClicked()
{
	GtkAddMagnetLinkWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r)
	{
	case Gtk::RESPONSE_OK:
		shared_ptr<Torrent> t = m_core->addTorrent(d.getMagnetURL());
		if (t)//Checks if t is not null
			m_treeview->addCell(t);
		//TODO Add error dialogue if torrent add is unsuccessful
		break;
	}
}

void GtkMainWindow::onPauseBtnClicked()
{
	m_treeview->setSelectedPaused(true);
}

void GtkMainWindow::onResumeBtnClicked()
{
	m_treeview->setSelectedPaused(false);
}

void GtkMainWindow::onRemoveBtnClicked()
{
	//get the torrent selected in treeview

	//remove the torrent from treeview
	//torrent.remove();
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
