// 24 march 2015
#include "simplesale.h"

// TODO the whole price system is not locale friendly

char *priceString(Price p)
{
	Price dollars, cents;

	dollars = p / 100;
	cents = p % 100;
	return g_strdup_printf("%" PriceFormat ".%02" PriceFormat, dollars, cents);
}
