/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define DEFINE_ACCELS(NAME, ...) static const char *NAME [] = { __VA_ARGS__ }
#define APPLICATION_FLAGS       G_APPLICATION_HANDLES_OPEN
#define SUPER_CLASS             viewer_application_parent_class

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
	gboolean debug;
};

/* キーボード ショートカット */
struct _ViewerApplicationAccelEntry
{
	const char *detailed_action_name;
	const char *const *accels;
};

typedef struct _ViewerApplicationAccelEntry ViewerApplicationAccelEntry;
static void activate (GApplication *application);
static void activate_new (GSimpleAction *action, GVariant *parameter, void *application);
static GtkWidget *create_document_window (ViewerApplication *application);
static void destroy (GtkWidget *widget, gpointer user_data);
static void load (ViewerDocumentWindow *window);
static void open (GApplication *application, GFile **files, int n_files, const char *hint);
static void startup (GApplication *application);
static void startup_accels (GtkApplication *application);
static void viewer_application_class_init (ViewerApplicationClass *self);
static void viewer_application_init (ViewerApplication *self);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
DEFINE_ACCELS (ACCELS_FULLSCREEN, "F11", NULL);
DEFINE_ACCELS (ACCELS_NEW, "<Ctrl>n", NULL);
DEFINE_ACCELS (ACCELS_QUIT, "<Ctrl>q", NULL);
DEFINE_ACCELS (ACCELS_SHORTCUTS, "<Ctrl>F1", "<Ctrl>question", "<Ctrl>slash", NULL);
DEFINE_ACCELS (ACCELS_UNFULLSCREEN, "Escape", NULL);

/* DEBUG オプション */
#define DEBUG_OPTION_LONG_NAME          "debug"
#define DEBUG_OPTION_SHORT_NAME         0
#define DEBUG_OPTION_FLAGS              G_OPTION_FLAG_NONE
#define DEBUG_OPTION_ARG                G_OPTION_ARG_NONE
#define DEBUG_OPTION_ARG_DATA           &self->debug
#define DEBUG_OPTION_DESCRIPTION        "Enable debug."
#define DEBUG_OPTION_ARG_DESCRIPTION    NULL

/* キーボード ショートカット */
static const ViewerApplicationAccelEntry ACCEL_ENTRIES [] =
{
	{ "app.new", ACCELS_NEW },
	{ "win.fullscreen", ACCELS_FULLSCREEN },
	{ "win.quit", ACCELS_QUIT },
	{ "win.show-help-overlay", ACCELS_SHORTCUTS },
	{ "win.unfullscreen", ACCELS_UNFULLSCREEN },
};

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ "new", activate_new, NULL, NULL, NULL, { 0 }},
};

/*
アプリケーションのウィンドウを表示します。
*/
static void activate (GApplication *application)
{
	GtkWindow *window;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));

	if (!window)
	{
		window = GTK_WINDOW (create_document_window (VIEWER_APPLICATION (application)));
	}

	gtk_window_present (window);
}

/*
新しいウィンドウを表示します。
*/
static void activate_new (GSimpleAction *action, GVariant *parameter, void *application)
{
	GtkWindow *window;
	window = GTK_WINDOW (create_document_window (VIEWER_APPLICATION (application)));
	gtk_window_present (window);
}

/*
ドキュメント ウィンドウを作成します。
作成したウィンドウに環境設定を適用します。
ウィンドウが破棄された時は環境設定を保存します。
*/
static GtkWidget *create_document_window (ViewerApplication *application)
{
	GtkWidget *widget;
	widget = viewer_document_window_new (GTK_APPLICATION (application));

	if (!application->debug)
	{
		g_signal_connect (widget, "destroy", G_CALLBACK (destroy), application);
		load (VIEWER_DOCUMENT_WINDOW (widget));
	}

	return widget;
}

/*
ウィンドウの環境設定を保存します。
*/
static void destroy (GtkWidget *widget, gpointer user_data)
{
	GSettings *settings;
	settings = g_settings_new (VIEWER_WINDOW_SETTINGS_ID);
	viewer_document_window_save_settings (VIEWER_DOCUMENT_WINDOW (widget), settings);
	g_object_unref (settings);
}

/*
ウィンドウに環境設定を適用します。
*/
static void load (ViewerDocumentWindow *window)
{
	GSettings *settings;
	settings = g_settings_new (VIEWER_WINDOW_SETTINGS_ID);
	viewer_document_window_load_settings (window, settings);
	g_object_unref (settings);
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

		if (VIEWER_IS_DOCUMENT_WINDOW (window) && !viewer_document_window_get_file (VIEWER_DOCUMENT_WINDOW (window)))
		{
			viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (window), files [n++]);
			gtk_window_present (window);
		}
		while (n < n_files)
		{
			window = GTK_WINDOW (create_document_window (VIEWER_APPLICATION (application)));
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
	gtk_window_set_default_icon_name (VIEWER_LOGO_ICON_NAME);
	g_action_map_add_action_entries (G_ACTION_MAP (application), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), application);
	startup_accels (GTK_APPLICATION (application));
}

/*
キーボード ショートカットを設定します。
*/
static void startup_accels (GtkApplication *application)
{
	const ViewerApplicationAccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

/*
新しいアプリケーションを作成します。
*/
GApplication *viewer_application_new (const char *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		"application-id", application_id,
		"flags", APPLICATION_FLAGS,
		NULL);
}

/*
コマンド ライン オプションを利用可能にします。
*/
void viewer_application_add_main_option_entries (ViewerApplication *self)
{
	const GOptionEntry entries [] =
	{
		{ OPTION_ENTRY_COLUMN (DEBUG_OPTION) },
		{ NULL },
	};

	g_application_add_main_option_entries (G_APPLICATION (self), entries);
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
