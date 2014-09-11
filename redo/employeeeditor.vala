// 11 september 2014

public class EmployeeEditor : ManagerTask {
	private DualPane dp;
	private Gtk.SearchEntry search;
	private Gtk.Button add;
	private Gtk.Button remove;
	private Gtk.Button save;
	private Gtk.Button cancel;

	private Employees employees;
	private Gtk.TreeView list;
	private Gtk.ScrolledWindow listScroller;

	private Gtk.Grid editorGrid;
	private Gtk.Entry name;
	private Gtk.Entry currentPassword;
	private Gtk.Entry newPassword;
	private Gtk.Entry confirmPassword;
	private Gtk.LevelBar strength;
	private Gtk.Label qualityMessage;
	private Gtk.Button changePassword;

	private bool selected;
	private Gtk.TreeIter selection;

	construct {
		Gtk.Label label;

		this.title = "simplesale";
		// TODO get rid of this
		this.destroy.connect(Gtk.main_quit);
		ScaleWindowUp(this, 5, 3);

		this.dp = new DualPane(this);
		this.dp.RightHeader.title = "Employee Editor";
		this.search = new Gtk.SearchEntry();
		this.search.placeholder_text = "Find existing employee";
		this.dp.LeftHeader.pack_start(this.search);
		// pack_end() is reversed
		this.remove = new Gtk.Button.from_icon_name("list-remove-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.remove);
		this.add = new Gtk.Button.from_icon_name("list-add-symbolic", Gtk.IconSize.BUTTON);
		this.dp.LeftHeader.pack_end(this.add);
		this.save = new Gtk.Button.with_label("Save Changes");
		this.save.get_style_context().add_class("suggested-action");
		this.dp.RightHeader.pack_start(this.save);
		this.cancel = new Gtk.Button.with_label("Cancel Changes");
		this.cancel.get_style_context().add_class("destructive-action");
		this.dp.RightHeader.pack_end(this.cancel);

		this.employees = new Employees.FromDB();
		this.list = new Gtk.TreeView();
		this.employees.SetupTreeView(this.list);
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

		this.editorGrid.attach_next_to(new Gtk.Separator(Gtk.Orientation.HORIZONTAL), label,
			Gtk.PositionType.BOTTOM, 3, 1);

		this.currentPassword = new Gtk.Entry();
		this.currentPassword.visibility = false;
		this.editorGrid.attach(this.currentPassword,
			0, 2, 1, 1);
		this.editorGrid.attach_next_to(newLabel("Current Password"), this.currentPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.newPassword = new Gtk.Entry();
		this.newPassword.visibility = false;
		this.editorGrid.attach_next_to(this.newPassword, this.currentPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.editorGrid.attach_next_to(newLabel("New Password"), this.newPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.confirmPassword = new Gtk.Entry();
		this.confirmPassword.visibility = false;
		this.editorGrid.attach_next_to(this.confirmPassword, this.newPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.editorGrid.attach_next_to(newLabel("Confirm New Password"), this.confirmPassword,
			Gtk.PositionType.LEFT, 1, 1);
		this.strength = new Gtk.LevelBar();
		this.editorGrid.attach_next_to(this.strength, this.confirmPassword,
			Gtk.PositionType.BOTTOM, 1, 1);
		this.editorGrid.attach_next_to(newLabel("New Password Strength"), this.strength,
			Gtk.PositionType.LEFT, 1, 1);
		this.qualityMessage = new Gtk.Label("");
		this.qualityMessage.wrap = true;
		this.qualityMessage.wrap_mode = Pango.WrapMode.WORD;
		this.qualityMessage.hexpand = true;
		this.qualityMessage.halign = Gtk.Align.FILL;
		this.editorGrid.attach_next_to(this.qualityMessage, this.strength,
			Gtk.PositionType.RIGHT, 1, 2);
		this.changePassword = new Gtk.Button.with_label("Change Password");
		this.editorGrid.attach_next_to(this.changePassword, this.strength,
			Gtk.PositionType.BOTTOM, 1, 1);

		this.dp.Add2(this.editorGrid);
	}
}
