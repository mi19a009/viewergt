/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define DEFINE_ACCELS(NAME, ...) static const char *NAME [] = { __VA_ARGS__ }
#define SUPER_CLASS viewer_application_parent_class

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
};

/* キーボード ショートカット */
struct _ViewerApplicationAccelEntry
{
	const char *detailed_action_name;
	const char *const *accels;
};

typedef struct _ViewerApplicationAccelEntry ACCELENTRY;
static void accelerate (GtkApplication *application);
static void activate (GApplication *application);
static void activate_new (GSimpleAction *action, GVariant *parameter, void *application);
static void open (GApplication *application, GFile **files, int n_files, const char *hint);
static void startup (GApplication *application);

static void viewer_application_class_init (ViewerApplicationClass *self);
static void viewer_application_init (ViewerApplication *self);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
DEFINE_ACCELS (ACCELS_NEW, "<Ctrl>n", NULL);
DEFINE_ACCELS (ACCELS_QUIT, "<Ctrl>q", NULL);
DEFINE_ACCELS (ACCELS_SHORTCUTS, "<Ctrl>F1", "<Ctrl>question", "<Ctrl>slash", NULL);

/* キーボード ショートカット */
static const ACCELENTRY ACCEL_ENTRIES [] =
{
	{ "app.new", ACCELS_NEW },
	{ "win.quit", ACCELS_QUIT },
	{ "win.show-help-overlay", ACCELS_SHORTCUTS },
};

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ "new", activate_new, NULL, NULL, NULL, { 0 }},
};

/*
キーボード ショートカットを設定します。
*/
static void accelerate (GtkApplication *application)
{
	const ACCELENTRY *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

/*
アプリケーションのウィンドウを表示します。
*/
static void activate (GApplication *application)
{
	GtkWindow *window;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));

	if (!window)
	{
		window = GTK_WINDOW (viewer_document_window_new (application));
	}

	gtk_window_present (window);
}

/*
新しいウィンドウを表示します。
*/
static void activate_new (GSimpleAction *action, GVariant *parameter, void *application)
{
	GtkWindow *window;
	window = GTK_WINDOW (viewer_document_window_new (G_APPLICATION (application)));
	gtk_window_present (window);
}

/*
ファイルの名前を指定してウィンドウを表示します。
*/
static void open (GApplication *application, GFile **files, int n_files, const char *hint)
{
	GtkWindow *window;
	int n;

	if (0 < n_files)
	{
		window = gtk_application_get_active_window (GTK_APPLICATION (application));
		n = 0;

		if (window && !viewer_document_window_get_file (VIEWER_DOCUMENT_WINDOW (window)))
		{
			viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (window), files [n++]);
			gtk_window_present (window);
		}
		while (n < n_files)
		{
			window = GTK_WINDOW (viewer_document_window_new (application));
			viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (window), files [n++]);
			gtk_window_present (window);
		}
	}
}

/*
メニュー項目のアクションを登録します。
キーボード ショートカットを登録します。
*/
static void startup (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->startup (application);
	g_action_map_add_action_entries (G_ACTION_MAP (application), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), application);
	accelerate (GTK_APPLICATION (application));
}

/*
新しいアプリケーションを作成します。
*/
GApplication *viewer_application_new (const char *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		"application-id", application_id,
		"flags", G_APPLICATION_HANDLES_OPEN,
		NULL);
}

/*
クラスを初期化します。
*/
static void viewer_application_class_init (ViewerApplicationClass *self)
{
	G_APPLICATION_CLASS (self)->activate = activate;
	G_APPLICATION_CLASS (self)->open = open;
	G_APPLICATION_CLASS (self)->startup = startup;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_application_init (ViewerApplication *self)
{
}
