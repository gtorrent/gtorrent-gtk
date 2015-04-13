#pragma once

#include <vector>
#include <gtkmm.h>
#include <gtorrent/Core.hpp>
#include <gtorrent/Torrent.hpp>

class GTorrentApp : public Gtk::Application
{
public:
	GTorrentApp();
	~GTorrentApp();

	static std::shared_ptr<GTorrentApp> getApp();
	static std::shared_ptr<gt::Core> getCore();

protected:
	virtual void on_startup();
	virtual void on_activate();
	virtual void on_open(const type_vec_files& files, const Glib::ustring& hint);

private:
	std::shared_ptr<gt::Core> core;
};