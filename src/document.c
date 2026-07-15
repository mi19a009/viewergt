/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ACTION_ABOUT                    "show-about"
#define ACTION_CLOSE                    "quit"
#define ACTION_FULLSCREEN               "fullscreen"
#define ACTION_UNFULLSCREEN             "unfullscreen"
#define SUPER_CLASS                     viewer_document_window_parent_class
#define WINDOW_DEFAULT_HEIGHT           400
#define WINDOW_DEFAULT_WIDTH            600
#define WINDOW_SETTINGS_MAXIMIZED       "maximized"
#define WINDOW_SETTINGS_SIZE            "size"
#define WINDOW_SETTINGS_SIZE_FORMAT     "(ii)"
#define WINDOW_STATE_WITH               (GDK_WINDOW_STATE_WITHDRAWN | GDK_WINDOW_STATE_ICONIFIED | GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_STICKY | GDK_WINDOW_STATE_FULLSCREEN)
#define WINDOW_TEMPLATE_NAME            "/com/github/mi19a009/PictureViewer/gtk/document.ui"
#define WINDOW_TITLE_CCH                256
#define WINDOW_TITLE_FORMAT             "%s - %s"
#define WINDOW_IS_FULLSCREEN(window)    (((window)->state & GDK_WINDOW_STATE_FULLSCREEN) != 0)
#define WINDOW_IS_MAXIMIZED(window)     (((window)->state & GDK_WINDOW_STATE_MAXIMIZED) != 0)

/* クラスのプロパティ */
enum _ViewerDocumentWindowProperties
{
	INVALID_PROPERTY_ID,
	FILE_PROPERTY_ID,
	PIXBUF_PROPERTY_ID,
};

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GtkWidget           *area; /* 描画領域 */
	GFile               *file; /* ユーザーが開いたファイル */
	GdkPixbuf           *pixbuf; /* ユーザーが開いたドキュメント */
	int                  state; /* 現在のウィンドウの状態 */
	int                  width; /* ウィンドウ化した場合のウィンドウの幅 */
	int                  height; /* ウィンドウ化した場合のウィンドウの高さ */
};

static void       activate_about                    (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       activate_close                    (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       activate_fullscreen               (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       activate_unfullscreen             (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       dispose                           (GObject *object);
static gboolean   draw                              (GtkWidget *widget, cairo_t *cairo, gpointer user_data);
static void       draw_document                     (ViewerDocumentWindow *window, cairo_t *cairo);
static void       get_property                      (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void       set_property                      (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void       size_allocate                     (GtkWidget *widget, GtkAllocation *allocation);
static void       update_area                       (ViewerDocumentWindow *window);
static void       update_fullscreen_state           (ViewerDocumentWindow *window);
static void       update_pixbuf                     (ViewerDocumentWindow *window);
static void       update_window_size                (ViewerDocumentWindow *window);
static void       update_window_state               (ViewerDocumentWindow *window, const GdkEventWindowState *event);
static void       update_window_title               (ViewerDocumentWindow *window);
static gboolean   window_state_event                (GtkWidget *widget, GdkEventWindowState *event);
static void       viewer_document_window_class_init (ViewerDocumentWindowClass *window);
static void       viewer_document_window_init       (ViewerDocumentWindow *window);

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
	{ ACTION_ABOUT,        activate_about,        NULL, NULL,    NULL },
	{ ACTION_CLOSE,        activate_close,        NULL, NULL,    NULL },
	{ ACTION_FULLSCREEN,   activate_fullscreen,   NULL, "false", NULL },
	{ ACTION_UNFULLSCREEN, activate_unfullscreen, NULL, NULL,    NULL },
};

/*
バージョン情報を表示します。
*/
static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	viewer_about_dialog_run (GTK_WINDOW (user_data));
}

/*
ウィンドウを閉じます。
*/
static void activate_close (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gtk_window_close (GTK_WINDOW (user_data));
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
ウィンドウが破棄される場合に呼び出されます。
ウィンドウのプロパティを破棄します。
*/
static void dispose (GObject *object)
{
	g_clear_object (&VIEWER_DOCUMENT_WINDOW (object)->file);
	g_clear_object (&VIEWER_DOCUMENT_WINDOW (object)->pixbuf);
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/*
領域が描画される場合に呼び出されます。
画像を描画します。
FALSE を返します。
*/
static gboolean draw (GtkWidget *widget, cairo_t *cairo, gpointer user_data)
{
	draw_document (VIEWER_DOCUMENT_WINDOW (user_data), cairo);
	return FALSE;
}

/*
現在のドキュメントを描画します。
*/
static void draw_document (ViewerDocumentWindow *window, cairo_t *cairo)
{
	if (window->pixbuf)
	{
		gdk_cairo_set_source_pixbuf (cairo, window->pixbuf, 0, 0);
		cairo_paint (cairo);
	}
}

/*
ウィンドウのプロパティを取得します。
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
ウィンドウのプロパティを設定します。
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
ウィンドウの大きさが変更された場合に呼び出されます。
ウィンドウの大きさを更新します。
*/
static void size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GTK_WIDGET_CLASS (SUPER_CLASS)->size_allocate (widget, allocation);
	update_window_size (VIEWER_DOCUMENT_WINDOW (widget));
}

static void update_area (ViewerDocumentWindow *window)
{
	if (window->area)
	{
		gtk_widget_queue_draw (window->area);
	}
}

/*
フルスクリーン時はメニュー項目にチェックを付けます。
*/
static void update_fullscreen_state (ViewerDocumentWindow *window)
{
	GAction *action;
	gboolean state;
	action = g_action_map_lookup_action (G_ACTION_MAP (window), ACTION_FULLSCREEN);

	if (G_IS_SIMPLE_ACTION (action))
	{
		state = WINDOW_IS_FULLSCREEN (window);
		g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (state));
	}
}

static void update_pixbuf (ViewerDocumentWindow *window)
{
	GFileInputStream *stream;
	GError *error;
	error = NULL;
	stream = g_file_read (window->file, NULL, &error);

	if (window->pixbuf)
	{
		g_object_unref (window->pixbuf);
	}
	if (stream)
	{
		window->pixbuf = gdk_pixbuf_new_from_stream (G_INPUT_STREAM (stream), NULL, &error);
	}
	else
	{
		window->pixbuf = NULL;
	}
	if (error)
	{
		viewer_alert_dialog_run (GTK_WINDOW (window), error);
		g_error_free (error);
	}
}

/*
ウィンドウ化時はウィンドウの大きさを更新します。
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
	char *filename;
	char title [WINDOW_TITLE_CCH];

	if (window->file)
	{
		filename = g_file_get_basename (window->file);
		g_snprintf (title, WINDOW_TITLE_CCH, WINDOW_TITLE_FORMAT, filename, VIEWER_TITLE);
		gtk_window_set_title (GTK_WINDOW (window), title);
		g_free (filename);
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
	GTK_WIDGET_CLASS (window)->size_allocate = size_allocate;
	GTK_WIDGET_CLASS (window)->window_state_event = window_state_event;
	OBJECT_CLASS_INSTALL_PROPERTY (G_OBJECT_CLASS (window), FILE_PROPERTY, PARAM_SPEC_OBJECT);
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (window), WINDOW_TEMPLATE_NAME);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (window), ViewerDocumentWindow, area);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (window), draw);
}

/*
開いたドキュメントのファイルを返します。
この値を開放してはならない。
*/
GFile *viewer_document_window_get_file (ViewerDocumentWindow *window)
{
	return window->file;
}

/*
開いたドキュメントの画像を返します。
この値を開放してはならない。
*/
GdkPixbuf *viewer_document_window_get_pixbuf (ViewerDocumentWindow *window)
{
	return window->pixbuf;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *window)
{
	window->width = WINDOW_DEFAULT_WIDTH;
	window->height = WINDOW_DEFAULT_HEIGHT;
	g_action_map_add_action_entries (G_ACTION_MAP (window), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), window);
	gtk_widget_init_template (GTK_WIDGET (window));
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
開いたドキュメントのファイルを指定します。
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
		update_pixbuf (window);
		update_area (window);
	}
}

/*
開いたドキュメントの画像を指定します。
*/
void viewer_document_window_set_pixbuf (ViewerDocumentWindow *window, GdkPixbuf *pixbuf)
{
	if (window->pixbuf != NULL)
	{
		if (window->pixbuf)
		{
			g_object_unref (window->pixbuf);
		}
		if (pixbuf)
		{
			window->pixbuf = g_object_ref (pixbuf);
		}
		else
		{
			window->pixbuf = NULL;
		}
	}
}
