// 6 september 2014

[SimpleType]
public struct Price : uint64 {
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
	public byte[] ToBytes()
	{
		byte[] out;
		int i;

		out = new byte[8];
		for (i = 7; i >= 0; i--) {
			out[i] = (uint8_t) (price & 0xFF);
			price >>= 8;
		}
		return out
	}
}
