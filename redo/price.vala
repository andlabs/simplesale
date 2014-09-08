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
		return ("$%" + Price.FORMAT + ".%02" + Price.FORMAT).printf(dollars, cents);
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

	public Price Price {
		get {
			return price;
		}
		set {
			string s;

			price = value;
			s = price.to_string();
			this.text = s[1:s.length];			// strip $
			this.changed();					// and update
			// TODO fast-track validation?
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
		this.changed.connect(() => {
			string icon;

			icon = null;
			this.valid = this.validate();
			if (!this.valid)
				icon = "dialog-error";
			this.set_icon_from_icon_name(Gtk.EntryIconPosition.PRIMARY, name);
		});
	}
}

public class PriceRenderer : Gtk.CellRendererText {
	private Price _price;
	public Price price {
		get {
			return _price;
		}
		set {
			_price = value;
			this.text = _price.to_string();
		}
	}
}
