/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define SUPER_CLASS                     viewer_document_window_parent_class
#define WINDOW_DEFAULT_HEIGHT           400
#define WINDOW_DEFAULT_WIDTH            600
#define WINDOW_SETTINGS_MAXIMIZED       "maximized"
#define WINDOW_SETTINGS_SIZE            "size"
#define WINDOW_SETTINGS_SIZE_FORMAT     "(ii)"
#define WINDOW_STATE_WITH               (GDK_WINDOW_STATE_WITHDRAWN | GDK_WINDOW_STATE_ICONIFIED | GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_STICKY | GDK_WINDOW_STATE_FULLSCREEN)
#define WINDOW_IS_FULLSCREEN(window) (((window)->state & GDK_WINDOW_STATE_FULLSCREEN) != 0)
#define WINDOW_IS_MAXIMIZED(window) (((window)->state & GDK_WINDOW_STATE_MAXIMIZED) != 0)

/* クラスのプロパティ */
enum _ViewerDocumentWindowProperties
{
	NULL_PROPERTY_ID,
	FILE_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GFile *file;
	int state;
	int width;
	int height;
};

static void     activate_about                    (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     activate_fullscreen               (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     activate_quit                     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     activate_unfullscreen             (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void     destroy                           (GtkWidget *widget);
static void     dispose                           (GObject *object);
static void     get_property                      (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void     set_property                      (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void     size_allocate                     (GtkWidget *widget, GtkAllocation *allocation);
static void     update_fullscreen_state           (ViewerDocumentWindow *window);
static void     update_window_size                (ViewerDocumentWindow *window);
static void     update_window_state               (ViewerDocumentWindow *window, const GdkEventWindowState *event);
static void     update_window_title               (ViewerDocumentWindow *window);
static gboolean window_state_event                (GtkWidget *widget, GdkEventWindowState *event);
static void     viewer_document_window_class_init (ViewerDocumentWindowClass *window);
static void     viewer_document_window_init       (ViewerDocumentWindow *window);

/* Viewer Document Window クラス */
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/* FILE プロパティ */
#define FILE_PROPERTY_NAME              "file"
#define FILE_PROPERTY_NICK              "File"
#define FILE_PROPERTY_BLURB             "File"
#define FILE_PROPERTY_OBJECT_TYPE       G_TYPE_FILE
#define FILE_PROPERTY_FLAGS             G_PARAM_READWRITE

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	{ "show-about",   activate_about,        NULL, NULL,    NULL },
	{ "fullscreen",   activate_fullscreen,   NULL, "false", NULL },
	{ "quit",         activate_quit,         NULL, NULL,    NULL },
	{ "unfullscreen", activate_unfullscreen, NULL, NULL,    NULL },
};

/*
バージョン情報を表示します。
*/
static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	viewer_about_dialog_run (GTK_WINDOW (user_data));
}

/*
ウィンドウを全画面表示します。
*/
static void activate_fullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	if (WINDOW_IS_FULLSCREEN (VIEWER_DOCUMENT_WINDOW (user_data)))
	{
		gtk_window_unfullscreen (GTK_WINDOW (user_data));
	}
	else
	{
		gtk_window_fullscreen (GTK_WINDOW (user_data));
	}
}

/*
ウィンドウを閉じます。
*/
static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gtk_window_close (GTK_WINDOW (user_data));
}

/*
全画面表示を解除します。
*/
static void activate_unfullscreen (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	if (WINDOW_IS_FULLSCREEN (VIEWER_DOCUMENT_WINDOW (user_data)))
	{
		gtk_window_unfullscreen (GTK_WINDOW (user_data));
	}
}

/*
ウィンドウを閉じます。
*/
static void destroy (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (SUPER_CLASS)->destroy (widget);
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
プロパティを取得します。
*/
static void get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case FILE_PROPERTY_ID:
		g_value_set_object (value, VIEWER_DOCUMENT_WINDOW (object)->file);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*
プロパティを設定します。
*/
static void set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id)
	{
	case FILE_PROPERTY_ID:
		viewer_document_window_set_file (VIEWER_DOCUMENT_WINDOW (object), g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}

/*
ウィンドウの大きさを更新します。
*/
static void size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GTK_WIDGET_CLASS (SUPER_CLASS)->size_allocate (widget, allocation);
	update_window_size (VIEWER_DOCUMENT_WINDOW (widget));
}

/*
フルスクリーン時はメニュー項目にチェックを付けます。
*/
static void update_fullscreen_state (ViewerDocumentWindow *window)
{
	GAction *action;
	gboolean state;
	action = g_action_map_lookup_action (G_ACTION_MAP (window), "fullscreen");

	if (G_IS_SIMPLE_ACTION (action))
	{
		state = WINDOW_IS_FULLSCREEN (window);
		g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (state));
	}
}

/*
ウィンドウの大きさを更新します。
*/
static void update_window_size (ViewerDocumentWindow *window)
{
	if ((window->state & WINDOW_STATE_WITH) == 0)
	{
		gtk_window_get_size (GTK_WINDOW (window), &window->width, &window->height);
	}
}

/*
ウィンドウの状態を更新します。
*/
static void update_window_state (ViewerDocumentWindow *window, const GdkEventWindowState *event)
{
	window->state = event->new_window_state;

	if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
	{
		update_fullscreen_state (window);
	}
}

/*
ウィンドウのタイトルを更新します。
*/
static void update_window_title (ViewerDocumentWindow *window)
{
	char *name;
	char buffer [256];

	if (window->file)
	{
		name = g_file_get_basename (window->file);
		g_snprintf (buffer, G_N_ELEMENTS (buffer), "%s - %s", name, VIEWER_TITLE);
		gtk_window_set_title (GTK_WINDOW (window), buffer);
		g_free (name);
	}
	else
	{
		gtk_window_set_title (GTK_WINDOW (window), VIEWER_TITLE);
	}
}

/*
ウィンドウのフルスクリーン化を追跡します。
*/
static gboolean window_state_event (GtkWidget *widget, GdkEventWindowState *event)
{
	gboolean status;
	status = GTK_WIDGET_CLASS (SUPER_CLASS)->window_state_event (widget, event);
	update_window_state (VIEWER_DOCUMENT_WINDOW (widget), event);
	return status;
}

/*
クラスを初期化します。
*/
static void viewer_document_window_class_init (ViewerDocumentWindowClass *window)
{
	G_OBJECT_CLASS (window)->dispose = dispose;
	G_OBJECT_CLASS (window)->get_property = get_property;
	G_OBJECT_CLASS (window)->set_property = set_property;
	GTK_WIDGET_CLASS (window)->destroy = destroy;
	GTK_WIDGET_CLASS (window)->size_allocate = size_allocate;
	GTK_WIDGET_CLASS (window)->window_state_event = window_state_event;
	OBJECT_CLASS_INSTALL_PROPERTY (G_OBJECT_CLASS (window), FILE_PROPERTY, PARAM_SPEC_OBJECT);
}

/*
ドキュメントのファイルを返します。
この値を開放してはならない。
*/
GFile *viewer_document_window_get_file (ViewerDocumentWindow *window)
{
	return window->file;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *window)
{
	window->width = WINDOW_DEFAULT_WIDTH;
	window->height = WINDOW_DEFAULT_HEIGHT;
	g_action_map_add_action_entries (G_ACTION_MAP (window), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), window);
}

/*
ウィンドウの設定を読み込んで適用します。
*/
void viewer_document_window_load_settings (ViewerDocumentWindow *window, GSettings *settings)
{
	g_settings_get (settings, WINDOW_SETTINGS_SIZE, WINDOW_SETTINGS_SIZE_FORMAT, &window->width, &window->height);
	gtk_window_set_default_size (GTK_WINDOW (window), window->width, window->height);

	if (g_settings_get_boolean (settings, WINDOW_SETTINGS_MAXIMIZED))
	{
		gtk_window_maximize (GTK_WINDOW (window));
	}
}

/*
新しいウィンドウを作成します。
*/
GtkWidget *viewer_document_window_new (GtkApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		"application",    application,
		"default-height", WINDOW_DEFAULT_HEIGHT,
		"default-width",  WINDOW_DEFAULT_WIDTH,
		"icon-name",      VIEWER_LOGO_ICON_NAME,
		"title",          VIEWER_TITLE,
		NULL);
}

/*
ウィンドウの設定を書き込みます。
*/
void viewer_document_window_save_settings (ViewerDocumentWindow *window, GSettings *settings)
{
	gboolean value;
	g_settings_set (settings, WINDOW_SETTINGS_SIZE, WINDOW_SETTINGS_SIZE_FORMAT, window->width, window->height);
	value = WINDOW_IS_MAXIMIZED (window);
	g_settings_set_boolean (settings, WINDOW_SETTINGS_MAXIMIZED, value);
}

/*
ドキュメントのファイルを指定します。
*/
void viewer_document_window_set_file (ViewerDocumentWindow *window, GFile *file)
{
	if (window->file != file)
	{
		if (window->file)
		{
			g_object_unref (window->file);
		}
		if (file)
		{
			window->file = g_object_ref (file);
		}
		else
		{
			window->file = NULL;
		}

		update_window_title (window);
	}
}
