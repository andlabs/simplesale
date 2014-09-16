// 9 september 2014

// what do you mean DB is too short wtf GObject
public class Database : GLib.Object {
	public Database()
	{
	}

	public int ItemCount()
	{
		return 4;
	}

	public string ItemName(int n)
	{
		switch (n) {
		case 0:
			return "Cheese Pizza";
		case 1:
			return "Cheese Slice";
		case 2:
			return "Soda";
		case 3:
			return "Cookie";
		}
		return "";
	}

	public Price ItemPrice(int n)
	{
		switch (n) {
		case 0:
			return 1000;
		case 1:
			return 200;
		case 2:
			return 125;
		case 3:
			return 100;
		}
		return 0;
	}

	public void ItemSetName(int index, string name)
	{
		// TODO
	}

	public void ItemSetPrice(int index, Price price)
	{
		// TODO
	}

	public int AppendItem(string name, Price price)
	{
		// TODO
		return this.ItemCount();
	}

	public void DeleteItem(int index)
	{
		// TODO
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
