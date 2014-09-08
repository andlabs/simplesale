// 6 september 2014

[SimpleType]
[IntegerType]
[CCode(has_type_id = false)]		// https://mail.gnome.org/archives/vala-list/2012-January/msg00105.html
public struct Price : uint64 {
	// weird scenario with vala here
	// static members are not inherited
	// but overriding throws up a warning about masking
	// probably an error in the spec?
	public new const string FORMAT = uint64.FORMAT;

	public string to_string()
	{
		Price dollars, cents;

		dollars = this / 100;
		cents = this % 100;
		return ("%" + Price.FORMAT + ".%02" + Price.FORMAT).printf(dollars, cents);
	}

	public string SignString()
	{
		return "$" + this.to_string()
	}

	// big-endian
	public static Price FromBytes(uint8[] bytes)
	{
		Price p = 0;
		int i;

		for (i = 0; i < 8; i++) {
			p <<= 8;
			p |= (Price) bytes[i];
		}
		return p;
	}

	// big-endian
	public uint8[] ToBytes()
	{
		uint8[] out;
		int i;
		Price p;

		out = new uint8[8];
		p = this;
		for (i = 7; i >= 0; i--) {
			out[i] = (uint8) (p & 0xFF);
			p >>= 8;
		}
		return out;
	}
}

public class PriceEntry : Gtk.Entry {
	private Price price;
	private bool valid;
	private ulong changedHandler;

	public Price Price {
		get {
			return price;
		}
		set {
			// temporarly kill the validation function; this is already valid
			GLib.SignalHandler.block(this, this.changedHandler);
			price = value;
			this.text = price.to_string();		// no $
			this.valid = true;
			this.updateIcon();
			GLib.SignalHandler.unblock(this, this.changedHandler);
		}
	}

	public bool Valid {
		get {
			return valid;
		}
	}

	private bool validate()
	{
		string amount;
		int n;
		int i;
		Price dollars = 0, cents = 0;

		amount = this.text;
		n = amount.length;
		if (n == 0)						// must enter something
			return false;
		if (n == 1 && amount[0] == '.')		// just a . is invalid
			return false;
		for (i = 0; i < n; i++) {
			if (amount[i] >= '0' && amount[i] <= '9') {
				dollars = dollars * 10 + (amount[i] - '0');
				continue;
			}
			if (amount[i] == '.')
				break;
			return false;				// unrecognized character
		}
		if (i < n) {						// amount[i] == '.'
			i++;
			if (i == n)					// ###. is valid
				;
			else if (i != n - 2)			// must have either none or two digits
				return false;
			else {
				cents = amount[i] - '0';
				cents = cents * 10 + (amount[i + 1] - '0');
			}
		}
		this.price = (dollars * 100) + cents;
		return true;
	}

	public PriceEntry()
	{
		this.width_chars = 10;		// not too long, not too short
		this.xalign = 1;
		this.changed.connect(() => {
			this.valid = this.validate();
			this.updateIcon();
		});
		// and get the ball rolling
		this.Price = 0;
	}

	private void updateIcon()
	{
		string icon;

		icon = null;
		if (!this.valid)
			icon = "dialog-error";
		this.set_icon_from_icon_name(Gtk.EntryIconPosition.PRIMARY, name);
	}
}

public class PriceRenderer : Gtk.CellRendererText {
	private Price _price;
	// TODO rename to Price
	public Price price {
		get {
			return _price;
		}
		set {
			_price = value;
			this.text = _price.SignString();
		}
	}
}
