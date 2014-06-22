// 17 june 2014
#include "simplesale.h"
#include <sqlite3.h>
#include "dbschema.h"

// files:
// - accounts: [ { "name": name, "password": [ bytes ] }, ... ]
// - items: name\nprice\nname\nprice\n...
// - orders: concatenation of { "time": unixtime, "customer": customerName, "amountPaid": price, "items": items }

#define DBFILE "db.sqlite3"

static sqlite3 *db;

#define ACCOUNTSNAME "accounts"
#define ITEMSNAME "items"
#define ORDERSNAME "orders"

#define SQLERR (sqlite3_errmsg(db))

void initDB(void)
{
	int err;
	int i;

	err = sqlite3_open_v2(DBFILE, &db, SQLITE_OPEN_READWRITE, NULL);
	if (err != SQLITE_OK) {		// may not exist; try creating
		int i;

		err = sqlite3_open_v2(DBFILE, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		if (err != SQLITE_OK)		// real error
			g_error("error opening database: %s", SQLERR);
		// needs creation
		for (i = 0; i < NVER; i++) {
			const char *p;
			const char *nextp;

			p = schemas[i];
			nextp = p;
			while (nextp != NULL && *nextp != '\0') {
				sqlite3_stmt *s;

				p = nextp;
				err = sqlite3_prepare(db, p, -1, &s, &nextp);
				if (err != SQLITE_OK)
					g_error("error preparing database initialization statement: %s", SQLERR);
				err = sqlite3_step(s);
				if (err != SQLITE_DONE)
					g_error("error executing database initialization statement: %s", SQLERR);
				err = sqlite3_finalize(s);
				if (err != SQLITE_OK)
					g_error("error finalizing database initialization statement: %s", SQLERR);
			}
		}
	}
	for (i = 0; stmts[i].query != NULL; i++) {
		err = sqlite3_prepare(db, stmts[i].query, -1, &stmts[i].stmt, NULL);
		if (err != SQLITE_OK)
			g_error("error preparing %s statement: %s", stmts[i].query, SQLERR);
	}
}

void endDB(void)
{
	int err;
	int i;

	for (i = 0; stmts[i].query != NULL; i++) {
		err = sqlite3_finalize(stmts[i].stmt);
		if (err != SQLITE_OK)
			g_error("error finalizing %s statement: %s", stmts[i].query, SQLERR);
	}
	err = sqlite3_close(db);
	if (err != SQLITE_OK)
		g_error("error closing database: %s\n", SQLERR);
}

// TRUE if a row is present, FALSE if not (that is, query done)
static gboolean run(int i)
{
	int err;

	err = sqlite3_step(stmts[i].stmt);
	if (err == SQLITE_ROW)
		return TRUE;
	if (err != SQLITE_DONE)
		g_error("error executing %s statement: %s", stmts[i].query, SQLERR);
	return FALSE;
}

static void reset(int i)
{
	int err;

	err = sqlite3_reset(stmts[i].stmt);
	if (err != SQLITE_OK)
		g_error("error resetting %s statement for next execution: %s", stmts[i].query, SQLERR);
}

static void bindBlob(int i, int arg, const void *blob, int n, void (*f)(void *))
{
	int err;

	err = sqlite3_bind_blob(stmts[i].stmt, arg, blob, n, f);
	if (err != SQLITE_OK)
		g_error("error binding blob argument %d of %s statement: %s", arg, stmts[i].query, SQLERR);
}

static void bindInt(int i, int arg, int n)
{
	int err;

	err = sqlite3_bind_int(stmts[i].stmt, arg, n);
	if (err != SQLITE_OK)
		g_error("error binding int argument %d of %s statement: %s", arg, stmts[i].query, SQLERR);
}

static void bindText(int i, int arg, const char *s, void (*f)(void *))
{
	int err;

	err = sqlite3_bind_text(stmts[i].stmt, arg, s, -1, f);
	if (err != SQLITE_OK)
		g_error("error binding text argument %d of %s statement: %s", arg, stmts[i].query, SQLERR);
}

static const void *blob(int i, int col)
{
	return sqlite3_column_blob(stmts[i].stmt, col);
}

static int blobsize(int i, int col)
{
	return sqlite3_column_bytes(stmts[i].stmt, col);
}

static int sqlint(int i, int col)
{
	return sqlite3_column_int(stmts[i].stmt, col);
}

void *dbGetSetting(const char *setting, int *len)
{
	void *out;
	int size;

	run(qBegin);
	reset(qBegin);
	bindText(qGetSetting, 1, setting, SQLITE_TRANSIENT);
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

struct dbIn {
};

static dbIn *newdbIn(void)
{
	return (dbIn *) g_malloc0(sizeof (dbIn));
}

dbIn *dbInOpenItems(void)
{
	run(qBegin);
	reset(qBegin);
	return newdbIn();
}

dbIn *dbInOpenAccounts(void)
{
	run(qBegin);
	reset(qBegin);
	return newdbIn();
}

gboolean dbInReadItem(dbIn *i, char **name, Price *price)
{
	USED(i);

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

gboolean dbInReadAccount(dbIn *i, char **name, char **password)
{
	USED(i);

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

void dbInCommitAndFree(dbIn *i)
{
	run(qCommit);
	reset(qCommit);
	g_free(i);
}

struct dbOut {
	int append;
	int count;
	int cur;
};

static dbOut *newdbOut(void)
{
	return (dbOut *) g_malloc0(sizeof (dbOut));
}

dbOut *dbOutOpenAndResetItems(void)
{
	dbOut *o;

	run(qBegin);
	reset(qBegin);
	run(qClearItems);
	reset(qClearItems);
	o = newdbOut();
	o->append = qAppendItem;
	return o;
}

dbOut *dbOutOpenForWritingAccounts(void)
{
	dbOut *o;

	run(qBegin);
	reset(qBegin);
	o = newdbOut();
	run(qGetAccountCount);
	o->count = sqlint(qGetAccountCount, 0);
	reset(qGetAccountCount);
	o->cur = 0;
	return o;
}

static gboolean writeItem(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	USED(path);

	dbOut *o = (dbOut *) data;
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
	return FALSE;
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

void dbOutCommitAndFree(dbOut *o)
{
	run(qCommit);
	reset(qCommit);
	g_free(o);
}
