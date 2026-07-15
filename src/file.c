/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define CANCEL_BUTTON_TEXT      TEXT ("_Cancel")
#define OPEN_BUTTON_TEXT        TEXT ("_Open")
#define OPEN_DIALOG_TITLE       TEXT ("Open File")
#define ALL_FILE_FILTER_NAME    "All Files"
#define IMAGE_FILE_FILTER_NAME  "Image Files"

/* ファイル フィルター */
typedef struct _ViewerFileFilterEntry
{
	const char *name;
	const char *const *patterns;
} ViewerFileFilterEntry;

/* すべてのファイル フィルター */
static const char *ALL_FILE_FILTER_PATTERNS [] =
{
	"*",
	NULL,
};

/* 画像ファイル フィルター */
static const char *IMAGE_FILE_FILTER_PATTERNS [] =
{
	"*.avif",
	"*.bmp",
	"*.gif"
	"*.jpg",
	"*.jpeg",
	"*.png",
	"*.webp",
	NULL,
};

/* ファイルを開くダイアログのフィルター */
static const ViewerFileFilterEntry OPEN_FILTER_ENTRIES [] =
{
	{ IMAGE_FILE_FILTER_NAME, IMAGE_FILE_FILTER_PATTERNS },
	{ ALL_FILE_FILTER_NAME,   ALL_FILE_FILTER_PATTERNS   },
};

/*
指定したファイル選択機能にフィルターを追加します。
*/
static void add (GtkFileChooser *chooser, const ViewerFileFilterEntry *entries, int n_entries)
{
	GtkFileFilter *filter;
	const char *const *patterns;
	int n;

	for (n = 0; n < n_entries; n++)
	{
		filter = gtk_file_filter_new ();
		patterns = entries->patterns;

		while (*patterns)
		{
			gtk_file_filter_add_pattern (filter, *patterns);
			patterns++;
		}

		gtk_file_filter_set_name (filter, TEXT (entries->name));
		gtk_file_chooser_add_filter (chooser, filter);
		entries++;
	}
}

/*
ファイル選択ダイアログを作成します。
*/
GtkWidget *viewer_file_chooser_dialog_new (GtkWindow *parent)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new (OPEN_DIALOG_TITLE, parent, GTK_FILE_CHOOSER_ACTION_OPEN, CANCEL_BUTTON_TEXT, GTK_RESPONSE_CANCEL, OPEN_BUTTON_TEXT, GTK_RESPONSE_ACCEPT, NULL);
	add (GTK_FILE_CHOOSER (dialog), OPEN_FILTER_ENTRIES, G_N_ELEMENTS (OPEN_FILTER_ENTRIES));
	return dialog;
}
