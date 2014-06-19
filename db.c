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
}

void endDB(void)
{
	int err;

	err = sqlite3_close(db);
	if (err != SQLITE_OK)
		g_error("error closing database: %s\n", SQLERR);
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

static GFileOutputStream *createdb(char *filename)
{
	GFile *f;
	GFileOutputStream *o;
	GError *err = NULL;

	f = g_file_new_for_path(filename);
	// FALSE for no backup
	o = g_file_replace(f, NULL, FALSE, G_FILE_CREATE_PRIVATE, NULL, &err);
	g_object_unref(f);
	if (o == NULL)
		g_error("error creating or appending to database file %s: %s", filename, err->message);
	return o;
}

static GDataOutputStream *dbToDataOutStream(GOutputStream *f)
{
	GDataOutputStream *w;

	w = g_data_output_stream_new(f);
	g_data_output_stream_set_byte_order(w, G_DATA_STREAM_BYTE_ORDER_BIG_ENDIAN);
	return w;
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
	char *filename;
	GOutputStream *o;
	GDataOutputStream *w;
};

static dbOut *newdbOut(char *filename)
{
	dbOut *o;

	o = (dbOut *) g_malloc0(sizeof (dbOut));
	o->filename = filename;
	o->o = g_memory_output_stream_new_resizable();
	o->w = dbToDataOutStream(o->o);
	return o;
}

void dbOutCreateAndFree(dbOut *o)
{
	GFileOutputStream *f;
	GMemoryOutputStream *m;
	gsize n;
	GError *err = NULL;

	f = createdb(o->filename);
	m = G_MEMORY_OUTPUT_STREAM(o->o);
	if (g_output_stream_write_all(G_OUTPUT_STREAM(f),
		g_memory_output_stream_get_data(m),
		g_memory_output_stream_get_data_size(m),
		&n, NULL, &err) == FALSE)
		g_error("error saving database file %s: %s", o->filename, err->message);
	g_object_unref(f);
	g_object_unref(o->w);
	g_object_unref(o->o);
	g_free(o);
}

// TODO dbOutAppendAndFree

static gboolean writeItem(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	USED(path);

	GDataOutputStream *w = (GDataOutputStream *) data;
	char *name;
	Price price;
	GError *err = NULL;

	gtk_tree_model_get(model, iter, 0, &name, 1, &price, -1);
	if (g_data_output_stream_put_string(w, name, NULL, &err) == FALSE)
		g_error("error writing item named \"%s\" to temporary storage: %s", name, err->message);
	if (g_data_output_stream_put_byte(w, '\n', NULL, &err) == FALSE)
		g_error("error writing newline for item \"%s\" to temporary storage: %s", name, err->message);
	if (PRICEWRITE(w, price, NULL, &err) == FALSE)
		g_error("error writing price for item \"%s\" to temporary storage: %s", name, err->message);
	return FALSE;
}

// for both items and orders
void dbOutWriteItemModel(GtkTreeModel *model, dbOut *o)
{
	gtk_tree_model_foreach(model, writeItem, o->w);
}

dbOut *dbOutOpenItems(void)
{
	return newdbOut(ITEMSNAME);
}
