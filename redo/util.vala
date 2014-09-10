// 10 september 2014

public void ScaleWindowUp(Gtk.Window win, double xscale, double yscale)
{
	int width, height;
	double w, h;

	win.get_size(out width, out height);
	w = (double) width;
	h = (double) height;
	w *= xscale;
	h *=  yscale;
	win.resize((int) w, (int) h);
}
