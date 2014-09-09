// 9 september 2014

public class DB : GLib.Object {
	public DB()
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
			i++;
			return true;
		case 1:
			name = "Cheese Slice";
			price = 200;
			i++;
			return true;
		case 2:
			name = "Soda";
			price = 125;
			i++;
			return true;
		case 3:
			name = "Cookie";
			price = 100;
			i++;
			return true;
		}
		return false;
	}

	public void EndGetItems()
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
