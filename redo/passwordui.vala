// 7 september 2014

public class PasswordPopover : Gtk.Popover {
	Gtk.Grid layout;
	Gtk.Entry password;
	Gtk.Button login;
	Gtk.Label incorrect;

	public PasswordPopover(Gtk.Widget relativeTo)
	{
		GLib.Object(relative_to: relativeTo);
		this.layout = new Gtk.Grid();
		this.password = new Gtk.Entry();
		this.password.visibility = false;
		this.password.caps_lock_warning = true;
		this.password.placeholder_text = "Enter password";
		this.layout.attach_next_to(this.password, null,
			Gtk.PositionType.RIGHT, 1, 1);
		this.login = new Gtk.Button.with_label("Log In");
		this.login.get_style_context().add_class("suggested-action");
		this.layout.attach_next_to(this.login, this.password,
			Gtk.PositionType.RIGHT, 1, 1);
		this.incorrect = new Gtk.Label("Password incorrect.");
		this.incorrect.halign = Gtk.Align.START;
		this.incorrect.no_show_all = true;
		this.layout.attach_next_to(this.incorrect, this.password,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.layout.row_spacing = 6;
		this.layout.column_spacing = 6;
		this.add(this.layout);
		this.border_width = 12;
		this.position = Gtk.PositionType.BOTTOM;		// always below
	}

	public void Open()
	{
		this.incorrect.hide();
		this.password.text = "";
		this.show_all();
	}
}
