#pragma once

#include <gtkmm/button.h>
#include <queue>
#include <cmath>

class GtkGraph : public Gtk::Button
{
public:
	GtkGraph(const unsigned maxSize = 3600);
	virtual ~GtkGraph();
	void add(std::shared_ptr<gt::Torrent> index, double upload, double download);
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
	const unsigned m_maxSize;
	static const unsigned m_labelLength = 40;
	double max(std::queue<double> q);
	std::queue<double> lastElements(std::queue<double> q, unsigned n);
	inline double max(std::queue<double> q1, std::queue<double> q2)
	{
		return max(q1) > max(q2) ? max(q1) : max(q2);
	}
	void draw(std::queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr);
};
