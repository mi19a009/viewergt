/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define SUPER_CLASS viewer_document_window_parent_class

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GFile *file;
};

static void activate_about (GSimpleAction *action, GVariant *parameter, void *document);
static void activate_quit (GSimpleAction *action, GVariant *parameter, void *document);
static void dispose (GObject *object);
static void update (ViewerDocumentWindow *document);

static void viewer_document_window_class_init (ViewerDocumentWindowClass *self);
static void viewer_document_window_init (ViewerDocumentWindow *self);

G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ "show-about", activate_about, NULL, NULL },
	{ "quit", activate_quit, NULL, NULL },
};

/*
バージョン情報を表示します。
*/
static void activate_about (GSimpleAction *action, GVariant *parameter, void *document)
{
	viewer_about_dialog_run (GTK_WINDOW (document));
}

/*
ウィンドウを閉じます。
*/
static void activate_quit (GSimpleAction *action, GVariant *parameter, void *document)
{
	gtk_window_close (GTK_WINDOW (document));
}

/*
プロパティを破棄します。
*/
static void dispose (GObject *object)
{
	g_clear_object (&VIEWER_DOCUMENT_WINDOW (object)->file);
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/*
ウィンドウのタイトルを更新します。
*/
static void update (ViewerDocumentWindow *document)
{
	char *name;
	char buffer [256];

	if (document->file)
	{
		name = g_file_get_basename (document->file);
		g_snprintf (buffer, G_N_ELEMENTS (buffer), "%s - %s", name, VIEWER_TITLE);
		gtk_window_set_title (GTK_WINDOW (document), buffer);
		g_free (name);
	}
	else
	{
		gtk_window_set_title (GTK_WINDOW (document), VIEWER_TITLE);
	}
}

/*
新しいウィンドウを作成します。
*/
GtkWidget *viewer_document_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		"application", application,
		NULL);
}

/*
クラスを初期化します。
*/
static void viewer_document_window_class_init (ViewerDocumentWindowClass *self)
{
	G_OBJECT_CLASS (self)->dispose = dispose;
}

/*
ドキュメントのファイルを返します。
この値を開放してはならない。
*/
GFile *viewer_document_window_get_file (ViewerDocumentWindow *self)
{
	return self->file;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *self)
{
	g_action_map_add_action_entries (G_ACTION_MAP (self), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), self);
	gtk_window_set_default_size (GTK_WINDOW (self), 640, 480);
	gtk_window_set_icon_name(GTK_WINDOW (self), VIEWER_LOGO_ICON_NAME);
	gtk_window_set_title (GTK_WINDOW (self), VIEWER_TITLE);
}

/*
ドキュメントのファイルを指定します。
*/
void viewer_document_window_set_file (ViewerDocumentWindow *self, GFile *file)
{
	if (self->file != file)
	{
		if (self->file)
		{
			g_object_unref (self->file);
		}
		if (file)
		{
			self->file = g_object_ref (file);
		}
		else
		{
			self->file = NULL;
		}

		update (self);
	}
}
