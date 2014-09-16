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
		// make the compiler happy
		name = "";
		price = 0;
		return false;
	}

	public void EndGetItems()
	{
	}

	public int EmployeeCount()
	{
		return 3;
	}

	public string EmployeeName(int n)
	{
		switch (n) {
		case 0:
			return "Name 1";
		case 1:
			return "Name 2";
		case 2:
			return "Name 3";
		}
		return "";
	}

	public void LogShiftStart(string name)
	{
	}

	public void LogShiftEnd(string name)
	{
	}

	public void LogOrder(string name, Order o)
	{
	}
}
