// 11 september 2014

public class EmployeeEditor : ManagerTask {
	private DualPane dp;
	private Gtk.SearchEntry search;
	private Gtk.Button add;
	private Gtk.Button remove;

	private Gtk.TreeView list;
	private Gtk.ScrolledWindow listScroller;

	private Gtk.Grid editorGrid;
	private Gtk.Entry name;
	private PasswordEditor pe;

	private bool selected;
	private Gtk.TreeIter selection;

	private ulong nameChangedHandler;

	construct {
		Gtk.Label label;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 5, 3);

		this.dp = new DualPane(this);
		this.dp.RightHeader.show_close_button = true;
		this.dp.RightHeader.title = "Employee Editor";
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find existing employee";
		this.dp.LeftHeader.pack_start(this.search);
		// pack_end() is reversed
		this.remove = new Gtk.Button.from_icon_name("list-remove-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.remove);
		this.add = new Gtk.Button.from_icon_name("list-add-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.add);

		this.list = new Gtk.TreeView();
		employees.SetupTreeView(this.list);
		this.list.set_search_entry(this.search);
		this.listScroller = new Gtk.ScrolledWindow(null, null);
		this.listScroller.shadow_type = Gtk.ShadowType.IN;
		this.listScroller.add(this.list);
		this.dp.Add1(this.listScroller);

		this.editorGrid = new Gtk.Grid();
		this.editorGrid.border_width = 12;
		this.editorGrid.row_spacing = 6;
		this.editorGrid.column_spacing = 12;
		this.name = new Gtk.Entry();
		this.name.hexpand = true;
		this.name.halign = Gtk.Align.FILL;
		this.editorGrid.attach_next_to(this.name, null,
			Gtk.PositionType.TOP, 2, 1);
		label = newLabel("Name");		// save for separator below
		this.editorGrid.attach_next_to(label, this.name,
			Gtk.PositionType.LEFT, 1, 1);

		// but first, add the password stuff
		this.pe = new PasswordEditor(this.editorGrid, this.name, true);

		this.editorGrid.insert_next_to(label, Gtk.PositionType.BOTTOM);
		this.editorGrid.attach_next_to(new Gtk.Separator(Gtk.Orientation.HORIZONTAL), label,
			Gtk.PositionType.BOTTOM, 3, 1);

		this.dp.Add2(this.editorGrid);

		this.name.bind_property("text",
			this.pe, "Name",
			GLib.BindingFlags.DEFAULT | GLib.BindingFlags.SYNC_CREATE);

		this.selected = false;
		this.list.get_selection().changed.connect(() => {
			this.selected = this.list.get_selection().get_selected(null, out this.selection);
			this.name.sensitive = this.selected;
			this.pe.Sensitive = this.selected;
			this.remove.sensitive = this.selected;
			this.pe.Reset();
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

		this.add.clicked.connect(() => {
			Gtk.MessageDialog prompt;
			Gtk.Box messageArea;

			prompt = new Gtk.MessageDialog(this, Gtk.DialogFlags.MODAL,
				Gtk.MessageType.QUESTION, Gtk.ButtonsType.OK_CANCEL,
				"Have the new Employee enter a password to create their account.");
			prompt.format_secondary_text("All Employees must have a password. Please ask the new employee to enter a password, and their account will be created.");
			prompt.image = new Gtk.Image.from_icon_name("dialog-password", Gtk.IconSize.DIALOG);
			pe = new PasswordEditor.AndGrid(false);
			messageArea = prompt.message_area as Gtk.Box;
			messageArea.pack_end(pe.Grid);
			pe.bind_property("Valid",
				prompt.get_widget_for_response(Gtk.ResponseType.OK), "sensitive",
				GLib.BindingFlags.DEFAULT | GLib.BindingFlags.SYNC_CREATE);
			// left-align everything
			messageArea.@foreach((widget) => {
				widget.halign = Gtk.Align.START;
			});
			// TODO doesn't look right
			prompt.show_all();
			prompt.run();
			// TODO
			prompt.destroy();
		});
	}
}
