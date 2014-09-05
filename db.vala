// 17 june 2014
using GLib;
using Gtk;
using Sqlite3;

// files:
// - accounts: [ { "name": name, "password": [ bytes ] }, ... ]
// - items: name\nprice\nname\nprice\n...
// - orders: concatenation of { "time": unixtime, "customer": customerName, "amountPaid": price, "items": items }

const string DBFILE = "db.sqlite3";

Sqlite3.Database db;

const string ACCOUNTSNAME = "accounts";
const string ITEMSNAME = "items";
const string ORDERSNAME = "orders";

public void initDB()
{
	int err;
	int i;

	err = Sqlite3.Database.open_v2(DBFILE, out db, Sqlite3.OPEN_READWRITE, null);
	if (err != Sqlite3.OK) {		// may not exist; try creating
		int i;

		err = Sqlite3.Database.open_v2(DBFILE, out db, Sqlite3.OPEN_READWRITE | Sqlite3.OPEN_CREATE, null);
		if (err != Sqlite3.OK)		// real error
			GLib.error("error opening database: %s", db.errmsg());
		// needs creation
		for (i = 0; i < len(schemas); i++) {
			Sqlite3.Statement s;

			err = db.prepare(schemas[i], schemas[i].length, out s);
			if (err != Sqlite3.OK)
				GLib.error("error preparing database initialization statement: %s", db.errmsg());
			err = s.step();
			if (err != Sqlite3.DONE)
				GLib.error("error executing database initialization statement: %s", db.errmsg());
			err = s.finalize();
			if (err != Sqlite3.OK)
				GLib.error("error finalizing database initialization statement: %s", db.errmsg());
		}
	}
	for (i = 0; i < stmt.length; i++) {
		err = db.prepare(stmts[i].query, stmts[i].query.length, out stmts[i].stmt);
		if (err != Sqlite3.OK)
			GLib.error("error preparing %s statement: %s", stmts[i].query, db.errmsg());
	}
}

public void endDB()
{
	int err;
	int i;

	for (i = 0; i < stmts.length; i++) {
		err = stmts[i].stmt.finalize();
		if (err != Sqlite3.OK)
			GLib.error("error finalizing %s statement: %s", stmts[i].query, db.errmsg());
	}
	err = db.close();
	if (err != Sqlite3.OK)
		GLib.error("error closing database: %s\n", db.errmsg());
}

// TRUE if a row is present, FALSE if not (that is, query done)
private bool run(int i)
{
	int err;

	err = stmts[i].stmt.step();
	if (err == Sqlite3.ROW)
		return true;
	if (err != Sqlite3.DONE)
		GLib.error("error executing %s statement: %s", stmts[i].query, db.errmsg());
	return false;
}

private void reset(int i)
{
	int err;

	err = stmts[i].stmt.reset();
	if (err != Sqlite3.OK)
		GLib.error("error resetting %s statement for next execution: %s", stmts[i].query, db.errmsg());
}

private void bindBlob(int i, int arg, const void *blob, int n, GLib.DestroyNotify f)
{
	int err;

	err = stmts[i].stmt.bind_blob(arg, blob, n, f);
	if (err != Sqlite3.OK)
		GLib.error("error binding blob argument %d of %s statement: %s", arg, stmts[i].query, db.errmsg());
}

private void bindInt(int i, int arg, int n)
{
	int err;

	err = stmts[i].stmt.bind_int(arg, n);
	if (err != Sqlite3.OK)
		GLib.error("error binding int argument %d of %s statement: %s", arg, stmts[i].query, db.errmsg());
}

private void bindText(int i, int arg, owned string s, GLib.DestroyNotify f)
{
	int err;

	err = stmts[i].stmt.bind_text(arg, s, -1, f);
	if (err != Sqlite3.OK)
		GLib.error("error binding text argument %d of %s statement: %s", arg, stmts[i].query, db.errmsg());
}

private const void *blob(int i, int col)
{
	return stmts[i].stmt.column_blob(col);
}

private int blobsize(int i, int col)
{
	return stmts[i].stmt.column_bytes(col);
}

private int sqlint(int i, int col)
{
	return stmts[i].stmt.column_int(col);
}

/*
TODO
public void *dbGetSetting(string setting, int *len)
{
	void *out;
	int size;

	run(qBegin);
	reset(qBegin);
	bindText(qGetSetting, 1, setting, Sqlite3.TRANSIENT);
	run(qGetSetting);
	size = blobsize(qGetSetting, 0);
	out = g_malloc0(size);
	memcpy(out, blob(qGetSetting, 0), size);
	if (len != NULL)
		*len = size;
	reset(qGetSetting);
	run(qCommit);
	reset(qCommit);
	return out;
}

void dbSetSetting(const char *setting, void *in, int size)
{
	run(qBegin);
	reset(qBegin);
	bindText(qSetSetting, 1, setting, SQLITE_TRANSIENT);
	bindBlob(qSetSetting, 2, in, size, SQLITE_TRANSIENT);
	run(qSetSetting);
	reset(qSetSetting);
	run(qCommit);
	reset(qCommit);
}
*/

public struct dbIn {
	public dbIn.OpenItems()
	{
		run(qBegin);
		reset(qBegin);
	}

	public dbIn,OpenAccounts()
	{
		run(qBegin);
		reset(qBegin);
	}

	/*
	TODO
	public bool ReadItem(char **name, Price *price)
	{
		int n;
		uint8_t pricebytes[8];

		if (run(qGetItems) == FALSE) {
			reset(qGetItems);
			return FALSE;
		}
		n = blobsize(qGetItems, 0);
		*name = (char *) g_malloc0((n + 1) * sizeof (char));
		memcpy(*name, blob(qGetItems, 0), n);
		memcpy(pricebytes, blob(qGetItems, 1), 8);
		*price = priceFromBytes(pricebytes);
		return TRUE;
	}

	public bool dbInReadAccount(char **name, char **password)
	{
		int n;

		if (run(qGetAccounts) == FALSE) {
			reset(qGetAccounts);
			return FALSE;
		}
		n = blobsize(qGetAccounts, 0);
		*name = (char *) g_malloc0((n + 1) * sizeof (char));
		memcpy(*name, blob(qGetAccounts, 0), n);
		n = blobsize(qGetAccounts, 1);
		*password = (char *) g_malloc0((n + 1) * sizeof (char));
		memcpy(*password, blob(qGetAccounts, 1), n);
		return TRUE;
	}
	*/

	public void Commit()
	{
		run(qCommit);
		reset(qCommit);
	}
}

struct dbOut {
	public int append;
	public int count;
	public int cur;

	public dbOut.OpenAndResetItems()
	{
		run(qBegin);
		reset(qBegin);
		run(qClearItems);
		reset(qClearItems);
		this.append = qAppendItem;
	}

	public dbOut.OpenForWritingAccounts()
	{
		run(qBegin);
		reset(qBegin);
		run(qGetAccountCount);
		this.count = sqlint(qGetAccountCount, 0);
		reset(qGetAccountCount);
		this.cur = 0;
	}

	/* TODO
	private bool writeItem(Gtk.TreeModel model, Gtk.TreePath path, Gtk.TreeIter iter)
	{
		char *name;
		Price price;
		uint8_t pricebytes[8];

		gtk_tree_model_get(model, iter, 0, &name, 1, &price, -1);
		bindBlob(o->append, 1, name, strlen(name), g_free);
		// the above calls g_free() already
		priceToBytes(price, pricebytes);
		bindBlob(o->append, 2, pricebytes, 8, SQLITE_TRANSIENT);
		run(o->append);
		reset(o->append);
		return false;
	}

	// for both items and orders
	void dbOutWriteItemModel(GtkTreeModel *model, dbOut *o)
	{
		gtk_tree_model_foreach(model, writeItem, o);
	}

	static gboolean writeAccount(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
	{
		USED(path);

		dbOut *o = (dbOut *) data;
		char *name, *password;

		gtk_tree_model_get(model, iter, 0, &name, 1, &password, -1);
		if (o->cur < o->count) {
			bindBlob(qChangeAccountInfo, 1, name, strlen(name), SQLITE_TRANSIENT);
			bindBlob(qChangeAccountInfo, 2, password, strlen(password), SQLITE_TRANSIENT);
			bindInt(qChangeAccountInfo, 3, o->cur + 1);		// rowid starts with 1
			run(qChangeAccountInfo);
			reset(qChangeAccountInfo);
		} else {
			bindBlob(qAddAccount, 1, name, strlen(name), SQLITE_TRANSIENT);
			bindBlob(qAddAccount, 2, password, strlen(password), SQLITE_TRANSIENT);
			run(qAddAccount);
			reset(qAddAccount);
		}
		o->cur++;
		return FALSE;
	}

	void dbOutWriteAccountsModel(GtkTreeModel *model, dbOut *o)
	{
		gtk_tree_model_foreach(model, writeAccount, o);
	}
	*/

	public void Commit()
	{
		run(qCommit);
		reset(qCommit);
	}
}
