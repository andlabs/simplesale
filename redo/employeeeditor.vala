// 11 september 2014

[GtkTemplate(ui = "/simplesale/employeeeditor.ui")]
public class EmployeeEditor : Gtk.Window, ManagerTask {
	[GtkChild(name = "list")]
	private Gtk.TreeView list;

	[GtkChild(name = "listSelection")]
	private Gtk.TreeSelection listSelection;

	[GtkChild(name = "addButton")]
	private Gtk.ToolButton addButton;

	[GtkChild(name = "removeButton")]
	private Gtk.ToolButton removeButton;

	[GtkChild(name = "search")]
	private Gtk.SearchEntry search;

	[GtkChild(name = "name")]
	private new Gtk.Entry name;

	[GtkChild(name = "changePassword")]
	private Gtk.Button changePassword;

	private bool selected;
	private Gtk.TreeIter selection;

	private ulong nameChangedHandler;

	public void Setup()
	{
		Gtk.HeaderBar hb;

		this.title = "simplesale";
		ScaleWindowUp(this, 1, 3);

		hb = new Gtk.HeaderBar();
		hb.show_close_button = true;
		hb.title = "Employee Editor";
		this.set_titlebar(hb);

		employees.SetupTreeView(this.list);
		this.list.set_search_entry(this.search);

		// this must come before the selected ones so the variable can be set for the initial signal
		this.nameChangedHandler = this.name.changed.connect(() => {
			if (!this.selected)
				GLib.error("remove employee button with no employee selected");
			employees.SetName(this.selection, this.name.text);
		});

		this.selected = false;
		this.list.get_selection().changed.connect(() => {
			this.selected = this.list.get_selection().get_selected(null, out this.selection);
			this.name.sensitive = this.selected;
			this.changePassword.sensitive = this.selected;
			this.removeButton.sensitive = this.selected;
			if (this.selected) {
				string n;

				employees.get(this.selection, 0, out n);
				this.name.text = n;
			} else {
				// prevent the GLib.error()s above from triggering when deselecting
				GLib.SignalHandler.block(this.name, this.nameChangedHandler);
				this.name.text = "";
				GLib.SignalHandler.unblock(this.name, this.nameChangedHandler);
			}
		});
		// and set initial value
		this.list.get_selection().changed();

		this.addButton.clicked.connect(() => {
			PasswordDialog prompt;
			int response;

			prompt = new PasswordDialog(this, false,
				"Have the new Employee enter a password to create their account.",
				"All Employees must have a password. Please ask the new employee to enter a password, and their account will be created.");
			prompt.show_all();
			response = prompt.run();
			if (response == Gtk.ResponseType.ACCEPT) {
				Gtk.TreeIter iter;

				employees.Append(out iter, prompt.New);
				this.list.get_selection().select_iter(iter);
			}
			prompt.destroy();
		});

		this.removeButton.clicked.connect(() => {
			if (!this.selected)
				GLib.error("remove employee button with no employee selected");
			// TODO confirm
			employees.Delete(this.selection);
		});

		this.changePassword.clicked.connect(() => {
			PasswordDialog prompt;
			int response;

			if (!this.selected)
				GLib.error("Change Password button with no employee selected");
			prompt = new PasswordDialog(this, true, "", "");
			prompt.show_all();
			for (;;) {
				response = prompt.run();
				if (response != Gtk.ResponseType.ACCEPT)
					break;
				if (employees.SetPassword(this.selection, prompt.Current, prompt.New) == true)
					break;
				prompt.Incorrect();
				// and continue
			}
			prompt.destroy();
		});
	}
}
