#pragma once

#include <queue>
#include <cmath>

#include <gtkmm/widget.h>
#include <gdkmm/general.h>
#include <gtk/gtkwidget.h>
#include <gtkmm/cssprovider.h>

#include "../Application.hpp"

class GtkGraph : public Gtk::Widget
{
public:
	GtkGraph(unsigned size = 61);
	virtual ~GtkGraph();

	void resize(unsigned size);
	void add(unsigned index, double upload, double download);
	void select(unsigned s);

protected:

	//Overrides:
	virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
	virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
	virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
	virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
	virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
	virtual void on_size_allocate(Gtk::Allocation& allocation);
	virtual void on_map();
	virtual void on_unmap();
	virtual void on_realize();
	virtual void on_unrealize();
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

	Glib::RefPtr<Gdk::Window> m_refGdkWindow;
private:
	vector<pair<queue<double>, queue<double>>> m_history;
	unsigned m_selected;
	unsigned m_maxSize;
	double max(queue<double> q);
	inline double max(queue<double> q1, queue<double> q2)
	{
		return max(q1) > max(q2) ? max(q1) : max(q2);
	}
	void draw(queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr);
};
