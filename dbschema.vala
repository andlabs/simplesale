// 19 june 2014
using Sqlite3;

// schema and prepared statements for sqlite3; separate file for organization's sake

const int NVER = 1;

private enum {
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

private enum {
	logActionAdded,
	logActionRemoved,
	logActionChangedName,
	logActionChangedPricePassword,
};

private string[] schemas = {
	"""CREATE TABLE settings (
		name TEXT PRIMARY KEY UNIQUE COLLATE NOCASE NOT NULL,
		value BLOB NOT NULL
	);""",
	"""INSERT INTO settings VALUES ('version', '1');""",
	"""CREATE TABLE items (
		name BLOB UNIQUE NOT NULL,
		price BLOB(8) NOT NULL		-- the (8) is ignored by sqlite
	);""",
	"""CREATE TABLE accounts (
		name BLOB UNIQUE NOT NULL,
		password BLOB NOT NULL
	);""",
	"""CREATE TABLE logentries (
		name BLOB NOT NULL,
		price BLOB(8) NOT NULL,
		action INTEGER NOT NULL,
		oldprice BLOB(8) NOT NULL
	);""",
	"""CREATE TABLE history (
		action INTEGER NOT NULL,
		account ROWID NOT NULL,		-- TODO
		time BLOB NOT NULL,
		param1 BLOB NOT NULL,
		param2 BLOB NOT NULL
	);""",
};

private enum {
	qBegin,
	qCommit,
	qGetSetting,
	qSetSetting,
	qGetItems,
	qClearItems,
	qAppendItem,
	qGetAccounts,
	qGetAccountCount,
	qChangeAccountInfo,
	qAddAccount,
};

private struct stmt {
	public string query;
	public Sqlite3.Statement stmt;

	stmt(query string)
	{
		this.query = query;
		this.stmt = null;
	}
};

private stmt[] stmts = {
	stmt("BEGIN EXCLUSIVE TRANSACTION;"),
	stmt("COMMIT;"),
	stmt("SELECT value FROM settings WHERE name = ?;"),
	stmt("REPLACE INTO settings VALUES (?, ?);"),
	stmt("SELECT * FROM items;"),
	stmt("DELETE FROM items;"),
	stmt("INSERT INTO items VALUES (?, ?);"),
	stmt("SELECT * FROM accounts;"),
	stmt("SELECT COUNT(*) FROM accounts;"),
	stmt("UPDATE accounts SET name = ?, password = ? WHERE rowid = ?;"),
	stmt("INSERT INTO accounts VALUES (?, ?);"),
};
