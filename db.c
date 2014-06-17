// 17 june 2014
#include "simplesale.h"

// files:
// - accounts: [ { "name": name, "password": [ bytes ] }, ... ]
// - items: name\nprice\nname\nprice\n...
// - orders: concatenation of { "time": unixtime, "customer": customerName, "amountPaid": price, "items": items }

#define ACCOUNTSNAME "accounts"
#define ITEMSNAME "items"
#define ORDERSNAME "orders"

void createDB(void)
{
	// TODO
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

void loadItems(void)
{
	GFileInputStream *i;
	GDataInputStream *r;
	GError *err = NULL;

	i = opendb(ITEMSNAME);
	r = dbToDataInStream(G_INPUT_STREAM(i));
	for (;;) {
		char *name;
		Price price;
		gsize len;

		name = g_data_input_stream_read_line_utf8(r, &len, NULL, &err);
		// gcc stop complainiing about if (...) if (...) ... else ... I know what I'm doing here
		if (name == NULL && err == NULL)		// end of file
			break;
		else if (name == NULL)
			g_error("error reading item name from items database: %s", err->message);
		price = (Price) PRICEREAD(r, NULL, &err);
		if (err != NULL)
			g_error("error reading item price from items database: %s", err->message);
		addItem(name, price);
		g_free(name);
	}
	g_object_unref(r);
	g_object_unref(i);
}

static gboolean writeItem(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	USED(path);

	GDataOutputStream *w = (GDataOutputStream *) data;
	char *name;
	Price price;
	GError *err = NULL;

	gtk_tree_model_get(model, iter, 0, &name, 1, &price, -1);
	if (g_data_output_stream_put_string(w, name, NULL, &err) == FALSE)
		g_error("error writing item named \"%s\" to items database: %s", name, err->message);
	if (g_data_output_stream_put_byte(w, '\n', NULL, &err) == FALSE)
		g_error("error writing newline for item \"%s\" to items database: %s", name, err->message);
	if (PRICEWRITE(w, price, NULL, &err) == FALSE)
		g_error("error writing price for item \"%s\" to item database: %s", name, err->message);
	return FALSE;
}

void saveItems(void)
{
	GFileOutputStream *o;
	GDataOutputStream *w;

	o = createdb(ITEMSNAME);
	w = dbToDataOutStream(G_OUTPUT_STREAM(o));
	gtk_tree_model_foreach(itemsModel(), writeItem, w);
	g_object_unref(w);
	g_object_unref(o);
}
