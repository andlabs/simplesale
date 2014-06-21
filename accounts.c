// 18 june 2014
#include "simplesale.h"
#define _OW_SOURCE
#include "ow-crypt.h"

static GtkListStore *accounts = NULL;
static GdkPixbuf *accountIcon = NULL;

#define BCRYPT_PREFIX ("$2y$")
#define BCRYPT_COUNT (10)		/* 2^10; must pass base-2 logarithm */
#define BCRYPT_N (256)

static char *gensalt(void)
{
	char *salt;
	char n[BCRYPT_N];
	FILE *f;

	f = fopen("/dev/urandom", "rb");		// TODO /dev/random is better but slower
	if (f == NULL)
		g_error("error opening /dev/urandom");
	if (fread(n, sizeof (char), BCRYPT_N, f) != BCRYPT_N)
		g_error("error reading from /dev/urandom");
	if (fclose(f) != 0)
		g_error("error closing /dev/urandom");
	salt = crypt_gensalt_ra(BCRYPT_PREFIX, BCRYPT_COUNT, n, BCRYPT_N);
	memset(n, 0, BCRYPT_N);		// just to be safe
	if (salt == NULL)
		g_error("crypt_gensalt_ra() returned NULL");
	return salt;
}

static char *hash(const char *password)
{
	char *salt;
	char *hashed;
	void *private = NULL;
	int n = 0;

	salt = gensalt();
	hashed = crypt_ra(password, salt, &private, &n);
	// hashed is actually part of private, so we can't free it just yet
	free(salt);
	if (hashed == NULL) {
		free(private);
		g_error("error hashing password");
	}
	hashed = g_strdup(hashed);
	free(private);
	// no need to free old hashed as that's part of private
	return hashed;
}

gboolean matches(const char *password, GtkTreeIter *iter)
{
	char *stored;
	char *input;
	size_t size;
	void *private = NULL;
	int n = 0;
	int result;

	gtk_tree_model_get(GTK_TREE_MODEL(accounts), iter, 1, &stored, -1);
	size = strlen(stored);
	input = crypt_ra(password, stored, &private, &n);
	result = memcmp(stored, input, size);
	free(private);
	// no need to free input as that's part of private
	return result == 0;
}

static GtkTreeIter appendAccount(char *name, char *hashed)
{
	GtkTreeIter iter;

	gtk_list_store_append(accounts, &iter);
	gtk_list_store_set(accounts, &iter,
		0, name,
		1, hashed,
		2, accountIcon,
		-1);
	return iter;
}

static void loadAccounts(void)
{
	dbIn *i;
	char *name, *password;

	i = dbInOpenAccounts();
	while (dbInReadAccount(i, &name, &password) == TRUE) {
		appendAccount(name, password);		// already hashed
		g_free(name);
		g_free(password);
	}
	dbInCommitAndFree(i);
}

void initAccounts(void)
{
	if (accounts != NULL)
		g_object_unref(accounts);
	accounts = gtk_list_store_new(3,
		G_TYPE_STRING,		// name
		G_TYPE_STRING,		// hashed password
		GDK_TYPE_PIXBUF);		// icon - TODO correct type?
	if (accountIcon == NULL)
		accountIcon = gtk_icon_theme_load_icon(
			gtk_icon_theme_get_default(),
			"face-smile",	// just a placeholder face for now; thanks Lumpio- in irc.freenode.net/#freedesktop
			48,
			0,			// no custom flags
			NULL);		// TODO handle error
	loadAccounts();
}

GtkTreeIter addAccount(char *name, char *password)
{
	GtkTreeIter iter;
	char *hashed;

	hashed = hash(password);
	iter = appendAccount(name, hashed);
	g_free(hashed);
	return iter;
}

char *accountName(GtkTreeIter *which)
{
	char *ret;

	gtk_tree_model_get(GTK_TREE_MODEL(accounts), which, 0, &ret, -1);
	return ret;
}

void setAccountName(GtkTreeIter *which, const char *name)
{
	gtk_list_store_set(accounts, which, 0, name, -1);
}

void setAccountPassword(GtkTreeIter *which, const char *password)
{
	char *hashed;

	hashed = hash(password);
	gtk_list_store_set(accounts, which, 1, hashed, -1);
	g_free(hashed);
}

void setAccountsModelAndIconLayout(GtkIconView *list)
{
	gtk_icon_view_set_model(list, GTK_TREE_MODEL(accounts));
	gtk_icon_view_set_text_column(list, 0);
	gtk_icon_view_set_pixbuf_column(list, 2);
}

void setAccountsModelAndColumnLayout(GtkTreeView *list)
{
	GtkCellRenderer *r;
	GtkTreeViewColumn *col;

	gtk_tree_view_set_model(list, GTK_TREE_MODEL(accounts));
	r = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("", r, "text", 0, NULL);
	gtk_tree_view_append_column(list, col);
	gtk_tree_view_set_headers_visible(list, FALSE);
}

void saveAccounts(void)
{
	dbOut *o;

	o = dbOutOpenForWritingAccounts();
	dbOutWriteAccountsModel(GTK_TREE_MODEL(accounts), o);
	dbOutCommitAndFree(o);
}

