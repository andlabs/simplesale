// 18 july 2014
#include "simplesale.h"

static const char *english[] = {
	"Pay Now",		// PayNow
	"Pay Later",		// PayLater
};

static const char **locales[] = {
	english,
};

const char *localstr(int lang, int str)
{
	return locales[lang][str];
}
