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
