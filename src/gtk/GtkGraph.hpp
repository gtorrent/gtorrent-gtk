#pragma once
#include <queue>
#include <cmath>

class GtkGraph : public Gtk::Widget
{
public:
	GtkGraph(unsigned size = 61);
	virtual ~GtkGraph();

	void resize(unsigned size);
    void add(std::shared_ptr<gt::Torrent> index, double upload, double download);
	void select(std::shared_ptr<gt::Torrent> s);

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
	std::map<std::shared_ptr<gt::Torrent>, std::pair<std::queue<double>, std::queue<double>>> m_history;
    std::shared_ptr<gt::Torrent> m_selected;
	unsigned m_maxSize;
	double max(std::queue<double> q);
	inline double max(std::queue<double> q1, std::queue<double> q2)
	{
		return max(q1) > max(q2) ? max(q1) : max(q2);
	}
	void draw(std::queue<double> q, double height, double increment, double maxValue, const Cairo::RefPtr<Cairo::Context>& cr);
};
