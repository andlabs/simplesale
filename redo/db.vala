// 9 september 2014

// what do you mean DB is too short wtf GObject
public class Database : GLib.Object {
	private GLib.ValueArray itemNames;
	private GLib.ValueArray itemPrices;
	private GLib.ValueArray employeeNames;
	private GLib.ValueArray employeePasswords;
	private uint16 kvendor;
	private uint16 kproduct;
	private string kserial;
	private uint16 rvendor;
	private uint16 rproduct;
	private string rserial;

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
		employeeNames = new GLib.ValueArray(0);
		employeeNames.append("Name 1");
		employeeNames.append("Name 2");
		employeeNames.append("Name 3");
		employeePasswords = new GLib.ValueArray(0);
		employeePasswords.append("");
		employeePasswords.append("password");
		employeePasswords.append("welp");
		// actually my laptop's touchpad
		this.kvendor = 0x413c;
		this.kproduct = 0x8158;
		this.kserial = "";
		// actually my laptop's keyboard
		this.rvendor = 0x413c;
		this.rproduct = 0x8157;
		this.rserial = "";
	}

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
		return (int) employeeNames.n_values;
	}

	public string EmployeeName(int n)
	{
		return (string) employeeNames.values[n];
	}

	public bool EmployeeCheckPassword(int n, string against)
	{
		return ((string) employeePasswords.values[n]) == against;
	}

	public void EmployeeSetName(int index, string name)
	{
		employeeNames.values[index] = name;
	}

	public bool EmployeeSetPassword(int index, string current, string password)
	{
		if (((string) employeePasswords.values[index]) == current) {
			employeePasswords.values[index] = password;
			return true;
		}
		return false;
	}

	public int AppendEmployee(string name, string password)
	{
		employeeNames.append(name);
		employeePasswords.append(password);
		return this.EmployeeCount() - 1;
	}

	public void DeleteEmployee(int index)
	{
		employeeNames.remove(index);
		employeePasswords.remove(index);
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

	public bool IsKitchenPrinter(uint16 vendor, uint16 product, string serial)
	{
		return (vendor == this.kvendor) &&
			(product == this.kproduct) &&
			(serial == this.kserial);
	}

	public void SetKitchenPrinter(uint16 vendor, uint16 product, string serial)
	{
		this.kvendor = vendor;
		this.kproduct = product;
		this.kserial = serial;
	}

	public bool IsReceiptPrinter(uint16 vendor, uint16 product, string serial)
	{
		return (vendor == this.rvendor) &&
			(product == this.rproduct) &&
			(serial == this.rserial);
	}

	public void SetReceiptPrinter(uint16 vendor, uint16 product, string serial)
	{
		this.rvendor = vendor;
		this.rproduct = product;
		this.rserial = serial;
	}
}
