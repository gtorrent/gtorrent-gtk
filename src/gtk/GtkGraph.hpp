#pragma once

#include <gtkmm/button.h>
#include <queue>
#include <cmath>
#include <gtkmm/builder.h>

class GtkGraph : public Gtk::Button
{
public:
	const Glib::RefPtr<Gtk::Builder> builder;
	GtkGraph(GtkButton *da, const Glib::RefPtr<Gtk::Builder> rbuilder, unsigned size = 61);
	virtual ~GtkGraph();
	void addValue(std::shared_ptr<gt::Torrent> index, double upload, double download);
	void select(std::shared_ptr<gt::Torrent> s);
	unsigned m_displaySize;
	inline void removeHistory(std::shared_ptr<gt::Torrent> t)
	{
		m_history.erase(t);
	}

protected:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
	virtual bool on_button_press_event(GdkEventButton* event);
private:

	std::map<std::shared_ptr<gt::Torrent>, std::pair<std::queue<double>, std::queue<double>>> m_history;
	std::shared_ptr<gt::Torrent> m_selected;
	unsigned m_maxSize;
	static const unsigned m_labelLength = 40;
	static const unsigned m_labelHeight = 10;
	double max(std::queue<double> q);
	std::queue<double> lastElements(std::queue<double> q, unsigned n);
	inline double max(std::queue<double> q1, std::queue<double> q2)
	{
		return max(q1) > max(q2) ? max(q1) : max(q2);
	}
	void draw(std::queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr);
};
