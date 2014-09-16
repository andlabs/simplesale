// 9 september 2014

// what do you mean DB is too short wtf GObject
public class Database : GLib.Object {
	public Database()
	{
		itemNames = new GLib.ValueArray(0);
		itemNames.append("Cheese Pizza");
		itemNames.append("Cheese Slice");
		itemNames.append("Soda");
		itemNames.append("Cookie");
		itemPrices = new GLib.ValueArray(0);
		itemPrices.append((Price) 1000);
		itemPrices.append((Price) 200);
		itemPrices.append((Price) 125);
		itemPrices.append((Price) 100);
	}

	private GLib.ValueArray itemNames;
	private GLib.ValueArray itemPrices;

	public int ItemCount()
	{
		return (int) itemNames.n_values;
	}

	public string ItemName(int n)
	{
		return (string) itemNames.values[n];
	}

	public Price ItemPrice(int n)
	{
		return (Price) itemPrices.values[n];
	}

	public void ItemSetName(int index, string name)
	{
		itemNames.values[index] = name;
	}

	public void ItemSetPrice(int index, Price price)
	{
		itemPrices.values[index] = price;
	}

	public int AppendItem(string name, Price price)
	{
		itemNames.append(name);
		itemPrices.append(price);
		return this.ItemCount() - 1;
	}

	public void DeleteItem(int index)
	{
		itemNames.remove(index);
		itemPrices.remove(index);
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
