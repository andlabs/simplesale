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

		this.login.clicked.connect(() => {
			if (Entered(this.password.text) == false) {
				this.password.text = "";
				this.incorrect.show();
				return;
			}
			this.destroy();
		});

		this.add(this.layout);
		this.layout.row_spacing = 6;
		this.layout.column_spacing = 6;
		this.border_width = 12;
		this.position = Gtk.PositionType.BOTTOM;		// always below
	}

	public void Open()
	{
		this.incorrect.hide();
		this.password.text = "";
		this.show_all();
	}

	public signal bool Entered(string password);
}

public class PasswordEditor : GLib.Object {
	private Gtk.Entry currentPassword;
	private Gtk.Entry newPassword;
	private Gtk.Entry confirmPassword;
	private Gtk.LevelBar strength;
	private Gtk.Label qualityMessage;
	private Gtk.Button changePassword;

	public Gtk.Grid Grid {
		get;
		private set;
	}

	public bool Valid {
		get;
		private set;
	}

	public bool Sensitive {
		set {
			if (this.currentPassword != null)
				this.currentPassword.sensitive = value;
			this.newPassword.sensitive = value;
			this.confirmPassword.sensitive = value;
			this.Valid = false;
		}
	}

	public string Name {
		get;
		set;
	}

	public string Current {
		get {
			return this.currentPassword.text;
		}
	}

	public string New {
		get {
			return this.newPassword.text;
		}
	}

	public PasswordEditor(Gtk.Grid grid, Gtk.Widget attachTo, bool changing)
	{
		this.Grid = null;
		this.buildGrid(grid, attachTo, changing);
		this.Reset();
	}

	public PasswordEditor.AndGrid(bool changing)
	{
		this.Grid = new Gtk.Grid();
		this.Grid.row_spacing = 6;
		this.Grid.column_spacing = 12;
		this.buildGrid(this.Grid, null, changing);
		this.Reset();
	}

	// TODO why does attachTo need to have a ? to work?
	private void buildGrid(Gtk.Grid grid, Gtk.Widget? attachTo, bool changing)
	{
		this.Name = "";

		this.currentPassword = null;
		if (changing) {
			this.currentPassword = new Gtk.Entry();
			this.currentPassword.visibility = false;
			grid.attach_next_to(this.currentPassword, attachTo,
				Gtk.PositionType.BOTTOM, 1, 1);
			grid.attach_next_to(newLabel("Current Password"), this.currentPassword,
				Gtk.PositionType.LEFT, 1, 1);
			this.currentPassword.changed.connect(this.checkPassword);
			attachTo = this.currentPassword;
		}

		this.newPassword = new Gtk.Entry();
		this.newPassword.visibility = false;
		grid.attach_next_to(this.newPassword, attachTo,
			Gtk.PositionType.BOTTOM, 1, 1);
		grid.attach_next_to(newLabel("New Password"), this.newPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.newPassword.changed.connect(this.checkPassword);

		this.confirmPassword = new Gtk.Entry();
		this.confirmPassword.visibility = false;
		grid.attach_next_to(this.confirmPassword, this.newPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		grid.attach_next_to(newLabel("Confirm New Password"), this.confirmPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.confirmPassword.changed.connect(this.checkPassword);

		this.strength = new Gtk.LevelBar();
		this.strength.min_value = 0;
		this.strength.max_value = 100;
		grid.attach_next_to(this.strength, this.confirmPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		grid.attach_next_to(newLabel("New Password Strength"), this.strength,
			Gtk.PositionType.LEFT, 1, 1);

		this.qualityMessage = new Gtk.Label("");
		this.qualityMessage.wrap = true;
		this.qualityMessage.wrap_mode = Pango.WrapMode.WORD;
		this.qualityMessage.hexpand = true;
		this.qualityMessage.halign = Gtk.Align.FILL;
		this.qualityMessage.xalign = 0;
		this.qualityMessage.valign = Gtk.Align.START;
		grid.attach_next_to(this.qualityMessage, this.strength,
			Gtk.PositionType.RIGHT, 1, 2);

		this.Valid = false;
		this.changePassword = null;
		if (changing) {
			this.changePassword = new Gtk.Button.with_label("Change Password");
			grid.attach_next_to(this.changePassword, this.strength,
				Gtk.PositionType.BOTTOM, 1, 1);
			this.changePassword.clicked.connect(() => {
				if (!this.Valid)
					GLib.error("Change Password button clicked while editor fields are invalid");
				this.Changed();
			});
			this.bind_property("Valid",
				this.changePassword, "sensitive",
				GLib.BindingFlags.DEFAULT | GLib.BindingFlags.SYNC_CREATE);
		}
	}

	private void checkPassword()
	{
		string? current;
		int quality;
		PasswordQuality.Error err;
		void *auxerror = null;
		bool allow = false;

		current = null;
		if (this.currentPassword != null)
			current = this.currentPassword.text;
		quality = (new PasswordQuality.Settings()).check(this.newPassword.text,
			current,
			this.Name,
			out auxerror);
		err = (PasswordQuality.Error) quality;
		if (quality < 0)
			quality = 0;
		this.strength.value = (double) quality;
		if (err < 0 || auxerror != null)
			this.qualityMessage.label = err.to_string(auxerror);
		else if (this.newPassword.text != this.confirmPassword.text)
			this.qualityMessage.label = "Passwords do not match.";
		else {
			this.qualityMessage.label = "";
			allow = true;
		}
		this.Valid = allow;
	}

	public void Reset()
	{
		if (this.currentPassword != null)
			this.currentPassword.text = "";
		this.newPassword.text = "";
		this.confirmPassword.text = "";
		this.strength.value = 0;
		this.qualityMessage.label = "";
		this.Valid = false;
	}

	public signal void Changed();
}
