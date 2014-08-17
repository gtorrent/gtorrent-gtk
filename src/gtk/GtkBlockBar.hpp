#pragma once

class GtkBlockBar : public Gtk::Widget
{
public:
	GtkBlockBar();
	virtual ~GtkBlockBar();
	void setBlocks(std::vector<bool> b);


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
	std::vector<bool> m_blocks;
};
