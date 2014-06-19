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

static GFileInputStream *opendb(char *filename)
{
	GFile *f;
	GFileInputStream *i;
	GError *err = NULL;

	f = g_file_new_for_path(filename);
	i = g_file_read(f, NULL, &err);
	g_object_unref(f);
	if (i == NULL)
		g_error("error opening database file %s: %s", filename, err->message);
	return i;
}

static GDataInputStream *dbToDataInStream(GInputStream *f)
{
	GDataInputStream *r;

	r = g_data_input_stream_new(f);
	g_data_input_stream_set_byte_order(r, G_DATA_STREAM_BYTE_ORDER_BIG_ENDIAN);
	g_data_input_stream_set_newline_type(r, G_DATA_STREAM_NEWLINE_TYPE_LF);
	return r;
}

struct dbIn {
	char *filename;
	GFileInputStream *i;
	GDataInputStream *r;
};

static dbIn *newdbIn(char *filename)
{
	dbIn *i;

	i = (dbIn *) g_malloc0(sizeof (dbIn));
	i->filename = filename;
	i->i = opendb(filename);
	i->r = dbToDataInStream(G_INPUT_STREAM(i->i));
	return i;
}

dbIn *dbInOpenItems(void)
{
	return newdbIn(ITEMSNAME);
}

gboolean dbInReadItem(dbIn *i, char **name, Price *price)
{
	char *n;		// to avoid overwriting on error
	Price p;
	gsize len;		// because the docs don't say if this can be NULL or not
	GError *err = NULL;

	n = g_data_input_stream_read_line_utf8(i->r, &len, NULL, &err);
	// gcc stop complainiing about if (...) if (...) ... else ... I know what I'm doing here
	if (n == NULL && err == NULL)		// end of file
		return FALSE;
	else if (n == NULL)
		g_error("error reading item name from database file %s: %s", i->filename, err->message);
	*name = n;
	p = (Price) PRICEREAD(i->r, NULL, &err);
	if (err != NULL)
		g_error("error reading item price from database file %s: %s", i->filename, err->message);
	*price = p;
	return TRUE;	
}

void dbInCloseAndFree(dbIn *i)
{
	g_object_unref(i->r);
	g_object_unref(i->i);
	g_free(i);
}

struct dbOut {
	int append;
};

static dbOut *newdbOut(void)
{
	return (dbOut *) g_malloc0(sizeof (dbOut));
}

dbOut *dbOutOpenAndResetItems(void)
{
	dbOut *o;

	run(qBegin);
	// TODO when do we reset?
	run(qClearItems);
	reset(qClearItems);
	o = newdbOut();
	o->append = qAppendItem;
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

void dbOutCommitAndFree(dbOut *o)
{
	run(qCommit);
	reset(qCommit);
	g_free(o);
}
