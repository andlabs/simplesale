// 7 september 2014

public class PasswordPopover : Gtk.Popover {
	private Gtk.Grid layout;
	private Gtk.Entry password;
	private Gtk.Button login;
	private Gtk.Label incorrect;

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

public class PasswordDialog : Gtk.Dialog {
	private Gtk.Grid layout;
	private Gtk.Label firstLabel;
	private Gtk.Entry currentPassword;
	private Gtk.Entry newPassword;
	private Gtk.Entry confirmPassword;
	private Gtk.LevelBar strength;
	private Gtk.Label qualityMessage;

	private Gtk.Button okButton;

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

	public PasswordDialog(Gtk.Window parent, bool changing, string primary, string secondary)
	{
		// we can't chain up to gtk_dialog_new_with_buttons(); fake it
		// thanks to b4n and ebassi in irc.gimp.net/#vala
		GLib.Object(transient_for: parent, modal: true);
		this.title = "Choose Password";
		if (changing)
			this.title = "Change Password";
		this.okButton = this.add_button("Confirm", Gtk.ResponseType.ACCEPT) as Gtk.Button;
		if (changing)
			this.okButton.label = "Change";
		this.add_button("Cancel", Gtk.ResponseType.CANCEL);
		this.border_width = 12;

		this.layout = new Gtk.Grid();
		this.layout.row_spacing = 6;
		this.layout.column_spacing = 12;

		// messages and current password field done at the end

		string pwstring;

		pwstring = "Password";
		if (changing)
			pwstring = "New Password";

		this.newPassword = new Gtk.Entry();
		this.newPassword.visibility = false;
		this.layout.attach_next_to(this.newPassword, null,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.firstLabel = newLabel(pwstring);
		this.layout.attach_next_to(this.firstLabel, this.newPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.newPassword.changed.connect(this.checkPassword);

		this.confirmPassword = new Gtk.Entry();
		this.confirmPassword.visibility = false;
		this.layout.attach_next_to(this.confirmPassword, this.newPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.layout.attach_next_to(newLabel("Confirm " + pwstring), this.confirmPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.confirmPassword.changed.connect(this.checkPassword);

		this.strength = new Gtk.LevelBar();
		this.strength.min_value = 0;
		this.strength.max_value = 100;
		this.layout.attach_next_to(this.strength, this.confirmPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.layout.attach_next_to(newLabel(pwstring + " Strength"), this.strength,
			Gtk.PositionType.LEFT, 1, 1);

		this.qualityMessage = new Gtk.Label("");
		this.qualityMessage.wrap = true;
		this.qualityMessage.wrap_mode = Pango.WrapMode.WORD;
		this.qualityMessage.hexpand = true;
		this.qualityMessage.halign = Gtk.Align.FILL;
		this.qualityMessage.xalign = 0;
		this.qualityMessage.valign = Gtk.Align.START;
		this.layout.attach_next_to(this.qualityMessage, this.strength,
			Gtk.PositionType.BOTTOM, 1, 1);

		// TODO space below the quality label

		this.currentPassword = null;
		if (changing) {
			this.currentPassword = new Gtk.Entry();
			this.currentPassword.visibility = false;
			this.layout.attach_next_to(this.currentPassword, this.newPassword,
				Gtk.PositionType.TOP, 1, 1);
			this.firstLabel = newLabel("Current Password");
			this.layout.attach_next_to(this.firstLabel, this.currentPassword,
				Gtk.PositionType.LEFT, 1, 1);
			this.currentPassword.changed.connect(this.checkPassword);
		}

		Gtk.Label label;

		// TODO these force the window to be super wide and I have no idea how to fix it
		if (secondary != "") {
			label = new Gtk.Label(secondary);
			label.wrap = true;
			label.wrap_mode = Pango.WrapMode.WORD;
			label.halign = Gtk.Align.START;
			this.layout.attach_next_to(label, this.firstLabel,
				Gtk.PositionType.TOP, 2, 1);
			this.firstLabel = label;
		}
		if (primary != "") {
			label = new Gtk.Label(primary);
			label.attributes = new Pango.AttrList();
			label.attributes.insert(Pango.attr_weight_new(Pango.Weight.BOLD));
			label.attributes.insert(Pango.attr_scale_new(Pango.Scale.LARGE));
			label.wrap = true;
			label.wrap_mode = Pango.WrapMode.WORD;
			label.halign = Gtk.Align.START;
			this.layout.attach_next_to(label, this.firstLabel,
				Gtk.PositionType.TOP, 2, 1);
			this.firstLabel = label;

			// have an icon if both is specified
			if (secondary != "") {
				Gtk.Image icon;

				icon = new Gtk.Image.from_icon_name("dialog-password", Gtk.IconSize.DIALOG);
				icon.valign = Gtk.Align.START;
				this.layout.attach_next_to(icon, this.firstLabel,
					Gtk.PositionType.LEFT, 1, 5);		// should be enough
			}
		}

		this.get_content_area().add(this.layout);
		this.reset("");
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
		this.okButton.sensitive = allow;
	}

	private void reset(string qualityMessage)
	{
		if (this.currentPassword != null)
			this.currentPassword.text = "";
		this.newPassword.text = "";
		this.confirmPassword.text = "";
		this.strength.value = 0;
		this.qualityMessage.label = qualityMessage;
		this.okButton.sensitive = false;
	}

	public void Incorrect()
	{
		this.reset("Current password incorrect.");
	}
}
