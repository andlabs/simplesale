// 9 september 2014

// what do you mean DB is too short wtf GObject
public class Database : GLib.Object {
	public Database()
	{
	}

	private int itemi;

	public void BeginGetItems()
	{
		this.itemi = 0;
	}

	public bool GetNextItem(out string name, out Price price)
	{
		switch (this.itemi) {
		case 0:
			name = "Cheese Pizza";
			price = 1000;
			this.itemi++;
			return true;
		case 1:
			name = "Cheese Slice";
			price = 200;
			this.itemi++;
			return true;
		case 2:
			name = "Soda";
			price = 125;
			this.itemi++;
			return true;
		case 3:
			name = "Cookie";
			price = 100;
			this.itemi++;
			return true;
		}
		return false;
	}

	public void EndGetItems()
	{
	}

	private int employeei;

	public void BeginGetEmployees()
	{
		this.employeei = 0;
	}

	public bool GetNextEmployee(out string name, out string password, out Gdk.Pixbuf icon)
	{
		password = "";
		icon = null;
		switch (this.employeei) {
		case 0:
			name = "Name 1";
			this.employeei++;
			return true;
		case 1:
			name = "Name 2";
			this.employeei++;
			return true;
		case 2:
			name = "Name 3";
			this.employeei++;
			return true;
		}
		return false;
	}

	public void EndGetEmployees()
	{
	}

	public void LogShiftStart()
	{
	}

	public void LogShiftEnd()
	{
	}

	public void LogOrder(Order o)
	{
	}
}
