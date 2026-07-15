/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_NEW              "new"
#define ACTION_OPEN             "open"
#define ACTION_PRINT            "print"
#define APPLICATION_FLAGS       G_APPLICATION_HANDLES_OPEN
#define SUPER_CLASS             viewer_application_parent_class
#define DEFINE_ACCELS(NAME, ...) static const char *NAME [] = { __VA_ARGS__ }

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication    parent_instance;
	GtkPrintSettings *settings;
	gboolean          debug;
};

/* キーボード ショートカット */
typedef struct _ViewerApplicationAccelEntry
{
	const char *detailed_action_name;
	const char *const *accels;
} ViewerApplicationAccelEntry;

static void       activate                      (GApplication *application);
static void       activate_new                  (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       activate_open                 (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       activate_print                (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static GtkWidget *create_document_window        (ViewerApplication *application);
static void       destroy                       (GtkWidget *widget, gpointer user_data);
static void       dispose                       (GObject *object);
static void       dispose_properties            (ViewerApplication *application);
static gboolean   has_file                      (GtkWindow *window);
static void       load                          (ViewerDocumentWindow *window);
static void       open                          (GApplication *application, GFile **files, int n_files, const char *hint);
static void       open_file                     (ViewerApplication *application);
static void       print                         (ViewerApplication *application);
static void       startup                       (GApplication *application);
static void       startup_accels                (GtkApplication *application);
static void       viewer_application_class_init (ViewerApplicationClass *application_class);
static void       viewer_application_init       (ViewerApplication *application);

/* Viewer Application クラス */
G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
DEFINE_ACCELS (ACCELS_FULLSCREEN, "F11", NULL);
DEFINE_ACCELS (ACCELS_NEW, "<Ctrl>n", NULL);
DEFINE_ACCELS (ACCELS_OPEN, "<Ctrl>o", NULL);
DEFINE_ACCELS (ACCELS_PRINT, "<Ctrl>p", NULL);
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
	{ "app.open", ACCELS_OPEN },
	{ "win.fullscreen", ACCELS_FULLSCREEN },
	{ "app.print", ACCELS_PRINT },
	{ "win.quit", ACCELS_QUIT },
	{ "win.show-help-overlay", ACCELS_SHORTCUTS },
	{ "win.unfullscreen", ACCELS_UNFULLSCREEN },
};

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ ACTION_NEW,   activate_new,   NULL, NULL, NULL },
	{ ACTION_OPEN,  activate_open,  NULL, NULL, NULL },
	{ ACTION_PRINT, activate_print, NULL, NULL, NULL },
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
static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWindow *window;
	window = GTK_WINDOW (create_document_window (VIEWER_APPLICATION (user_data)));
	gtk_window_present (window);
}

/*
ファイルを開くダイアログを表示します。
*/
static void activate_open (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	open_file (VIEWER_APPLICATION (user_data));
}

/*
印刷ダイアログを表示します。
*/
static void activate_print (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	print (VIEWER_APPLICATION (user_data));
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
プロパティを破棄します。
*/
static void dispose (GObject *object)
{
	dispose_properties (VIEWER_APPLICATION (object));
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/*
プロパティを破棄します。
*/
static void dispose_properties (ViewerApplication *application)
{
	g_clear_object (&application->settings);
}

/*
指定したウィンドウがファイルを参照している場合は TRUE を返します。
*/
static gboolean has_file (GtkWindow *window)
{
	return
		VIEWER_IS_DOCUMENT_WINDOW (window) &&
		viewer_document_window_get_file (VIEWER_DOCUMENT_WINDOW (window));
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

		if (!has_file (window))
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
ファイルを開くダイアログを表示します。
*/
static void open_file (ViewerApplication *application)
{
	GtkWindow *window;
	GtkWidget *dialog;
	GFile *file;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));
	dialog = viewer_file_chooser_dialog_new (window);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));

		if (has_file (window))
		{
			window = GTK_WINDOW (create_document_window (application));
		}

		viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (window), file);
		gtk_window_present (window);
		g_object_unref (file);
	}

	gtk_widget_destroy (dialog);
}

/*
印刷ダイアログを表示します。
*/
static void print (ViewerApplication *application)
{
	GtkWindow *window;
	GdkPixbuf *pixbuf;
	GtkPrintOperation *operation;
	GtkPrintSettings *settings;
	GError *error;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));

	if (VIEWER_IS_DOCUMENT_WINDOW (window))
	{
		pixbuf = viewer_document_window_get_pixbuf (VIEWER_DOCUMENT_WINDOW (window));

		if (pixbuf)
		{
			error = NULL;
			settings = application->settings;
			operation = viewer_print_operation_new ();
			viewer_print_operation_set_pixbuf (VIEWER_PRINT_OPERATION (operation), pixbuf);
			gtk_print_operation_set_embed_page_setup (operation, TRUE);
			gtk_print_operation_set_print_settings (operation, settings);
			gtk_print_operation_run (operation, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, window, &error);
			settings = gtk_print_operation_get_print_settings (operation);
			g_object_unref (operation);

			if (error)
			{
				viewer_alert_dialog_run (window, error);
				g_error_free (error);
			}
			if (settings)
			{
				viewer_application_set_print_settings (application, settings);
			}
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
static void viewer_application_class_init (ViewerApplicationClass *application_class)
{
	G_OBJECT_CLASS (application_class)->dispose = dispose;
	G_APPLICATION_CLASS (application_class)->activate = activate;
	G_APPLICATION_CLASS (application_class)->open = open;
	G_APPLICATION_CLASS (application_class)->startup = startup;
}

/*
印刷情報を取得します。
*/
GtkPrintSettings *viewer_application_get_print_settings (ViewerApplication *application)
{
	GtkPrintSettings *settings;

	if (application->settings)
	{
		settings = g_object_ref (application->settings);
	}
	else
	{
		settings = NULL;
	}

	return settings;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_application_init (ViewerApplication *application)
{
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
印刷情報を設定します。
*/
void viewer_application_set_print_settings (ViewerApplication *application, GtkPrintSettings *settings)
{
	if (application->settings != settings)
	{
		if (application->settings)
		{
			g_object_unref (application->settings);
		}
		if (settings)
		{
			application->settings = g_object_ref (settings);
		}
		else
		{
			application->settings = NULL;
		}
	}
}
