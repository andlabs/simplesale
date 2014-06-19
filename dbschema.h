// 19 june 2014

// schema and prepared statements for sqlite3; separate file for organization's sake

typedef const char *const schema;
#define SCHEMA static schema
#define NVER 1

enum {
	actionOrder,
	actionClockIn,
	actionClockOut,
	actionManagerClockIn,
	actionManagerClockOut,
	actionItemsChanged,
	actionAccountsChanged,
	actionDevicesChanged,
	actionMoneyWithdrawn,
	// TODO actionVersionUpgrade?
};

enum {
	logActionAdded,
	logActionRemoved,
	logActionChangedName,
	logActionChangedPricePassword,
};

SCHEMA schemas[NVER] = {
	"CREATE TABLE settings ("
	"	name TEXT PRIMARY KEY UNIQUE COLLATE NOCASE NOT NULL,"
	"	value BLOB NOT NULL"
	");"
	"INSERT INTO settings VALUES ('version', '1');"
	"CREATE TABLE items ("
	"	name BLOB UNIQUE NOT NULL,"
	"	price BLOB(8) NOT NULL"		// the (8) is ignored by sqlite
	");"
	"CREATE TABLE accounts ("
	"	name BLOB UNIQUE NOT NULL,"
	"	password BLOB NOT NULL"
	");"
	"CREATE TABLE logentries ("
	"	name BLOB NOT NULL,"
	"	price BLOB(8) NOT NULL,"
	"	action INTEGER NOT NULL,"
	"	oldprice BLOB(8) NOT NULL"
	");"
	"CREATE TABLE history ("
	"	action INTEGER NOT NULL,"
	"	account ROWID NOT NULL,"		// TODO
	"	time BLOB NOT NULL,"
	"	param1 BLOB NOT NULL,"
	"	param2 BLOB NOT NULL"
	");",
};

#undef SCHEMA
