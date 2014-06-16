// 16 june 2014
#include "simplesale.h"

gchar *priceToString(Price p, char *prefix)
{
	Price dollars, cents;

	dollars = p / 100;
	cents = p % 100;
	return g_strdup_printf("%s%" PRICEFMT ".%02" PRICEFMT, prefix, dollars, cents);
}
