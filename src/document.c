/*
Copyright (C) 2026 Taichi Murakami.
ドキュメント ウィンドウ クラスを実装します。
*/
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define ABOUT_ACTION_ACTIVATE           activate_about
#define ABOUT_ACTION_NAME               "show-about"
#define APPLICATION_PROPERTY            "application"
#define DEFAULT_HEIGHT                  400
#define DEFAULT_HEIGHT_PROPERTY         "default-height"
#define DEFAULT_WIDTH                   600
#define DEFAULT_WIDTH_PROPERTY          "default-width"
#define MAXIMIZED_SETTINGS_KEY          "maximized"
#define MENUBAR_PROPERTY                "show-menubar"
#define SIGNAL_SURFACE                  "notify::state"
#define SIZE_SETTINGS_FORMAT            "(ii)"
#define SIZE_SETTINGS_KEY               "size"
#define TITLE_PROPERTY                  "title"
#define WIDGET_TEMPLATE_NAME            (VIEWER_APPLICATION_PATH "/gtk/document.ui")
#define ACTION_ENTRY(ACTION)            { ACTION ##_NAME, ACTION ##_ACTIVATE }

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GtkWidget           *canvas;
	gint                 width;
	gint                 height;
	gboolean             maximized;
	gboolean             fullscreen;
};

static void activate_about        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void connect_surface       (GtkNative *native);
static void disconnect_surface    (GtkNative *native);
static void dispose               (GObject *object);
static void draw                  (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void get_property          (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void load                  (ViewerWindowSettings *window, GSettings *settings);
static void load_settings         (ViewerDocumentWindow *window, GSettings *settings);
static void realize               (GtkWidget *widget);
static void save                  (ViewerWindowSettings *window, GSettings *settings);
static void save_settings         (ViewerDocumentWindow *window, GSettings *settings);
static void set_property          (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void set_window_size       (ViewerDocumentWindow *window, int width, int height);
static void size_allocate         (GtkWidget *widget, int width, int height, int baseline);
static void surface_state_changed (GObject *object, GParamSpec *pspec, gpointer user_data);
static void unrealize             (GtkWidget *widget);
static void g_object_class_init                   (GObjectClass *object);
static void gtk_widget_class_init                 (GtkWidgetClass *widget);
static void viewer_document_window_class_init     (ViewerDocumentWindowClass *window);
static void viewer_document_window_init           (ViewerDocumentWindow *window);
static void viewer_window_settings_interface_init (ViewerWindowSettingsInterface *window);

/*
ドキュメント ウィンドウを表します。
ユーザーが開いた画像を格納します。
描画領域に画像を表示します。
*/
G_DEFINE_FINAL_TYPE_WITH_CODE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW,
G_IMPLEMENT_INTERFACE (VIEWER_TYPE_WINDOW_SETTINGS, viewer_window_settings_interface_init));

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	ACTION_ENTRY (ABOUT_ACTION),
};

/*
バージョン情報を表示します。
*/
static void activate_about (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = viewer_about_dialog_new ();
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (user_data));
	gtk_window_present (GTK_WINDOW (dialog));
}

static void connect_surface (GtkNative *native)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (native);
	g_signal_connect (surface, SIGNAL_SURFACE, G_CALLBACK (surface_state_changed), native);
}

static void disconnect_surface (GtkNative *native)
{
	GdkSurface *surface;
	surface = gtk_native_get_surface (native);
	g_signal_handlers_disconnect_by_func (surface, surface_state_changed, native);
}

/*
クラスのインスタンスを破棄します。
*/
static void dispose (GObject *object)
{
	gtk_widget_dispose_template (GTK_WIDGET (object), VIEWER_TYPE_DOCUMENT_WINDOW);
	G_OBJECT_CLASS (viewer_document_window_parent_class)->dispose (object);
}

/*
領域を描画します。
*/
static void draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	cairo_set_source_rgb (cairo, 0.125, 0.25, 0.5);
	cairo_rectangle (cairo, 0, 0, width, height);
	cairo_fill (cairo);
}

static void get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

/*
ウィンドウの設定を読み込んで適用します。
*/
static void load (ViewerWindowSettings *window, GSettings *settings)
{
	load_settings (VIEWER_DOCUMENT_WINDOW (window), settings);
}

static void load_settings (ViewerDocumentWindow *window, GSettings *settings)
{
	g_settings_get (settings, SIZE_SETTINGS_KEY, SIZE_SETTINGS_FORMAT, &window->width, &window->height);
	gtk_window_set_default_size (GTK_WINDOW (window), window->width, window->height);

	if (g_settings_get_boolean (settings, MAXIMIZED_SETTINGS_KEY))
	{
		gtk_window_maximize (GTK_WINDOW (window));
	}
}

static void realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->realize (widget);
	connect_surface (GTK_NATIVE (widget));
}

/*
ウィンドウの設定を書き込みます。
*/
static void save (ViewerWindowSettings *window, GSettings *settings)
{
	save_settings (VIEWER_DOCUMENT_WINDOW (window), settings);
}

static void save_settings (ViewerDocumentWindow *window, GSettings *settings)
{
	g_settings_set (settings, SIZE_SETTINGS_KEY, SIZE_SETTINGS_FORMAT, window->width, window->height);
	g_settings_set_boolean (settings, MAXIMIZED_SETTINGS_KEY, window->maximized);
}

static void set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void set_window_size (ViewerDocumentWindow *window, int width, int height)
{
	if (!window->maximized && !window->fullscreen)
	{
		window->width = width;
		window->height = height;
	}
}

static void size_allocate (GtkWidget *widget, int width, int height, int baseline)
{
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->size_allocate (widget, width, height, baseline);
	set_window_size (VIEWER_DOCUMENT_WINDOW (widget), width, height);
}

static void surface_state_changed (GObject *object, GParamSpec *pspec, gpointer user_data)
{
	ViewerDocumentWindow *window;
	GdkToplevelState state;
	state = gdk_toplevel_get_state (GDK_TOPLEVEL (object));
	window = VIEWER_DOCUMENT_WINDOW (user_data);
	window->maximized = (state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
	window->maximized = (state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
}

static void unrealize (GtkWidget *widget)
{
	disconnect_surface (GTK_NATIVE (widget));
	GTK_WIDGET_CLASS (viewer_document_window_parent_class)->unrealize (widget);
}

static void g_object_class_init (GObjectClass *object)
{
	object->dispose = dispose;
	object->get_property = get_property;
	object->set_property = set_property;
}

static void gtk_widget_class_init (GtkWidgetClass *widget)
{
	widget->realize = realize;
	widget->size_allocate = size_allocate;
	widget->unrealize = unrealize;
	gtk_widget_class_set_template_from_resource (widget, WIDGET_TEMPLATE_NAME);
	gtk_widget_class_bind_template_child (widget, ViewerDocumentWindow, canvas);
}

/*
クラスを初期化します。
*/
static void viewer_document_window_class_init (ViewerDocumentWindowClass *window)
{
	g_object_class_init (G_OBJECT_CLASS (window));
	gtk_widget_class_init (GTK_WIDGET_CLASS (window));
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *window)
{
	g_action_map_add_action_entries (G_ACTION_MAP (window), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), window);
	gtk_widget_init_template (GTK_WIDGET (window));
	gtk_window_get_default_size (GTK_WINDOW (window), &window->width, &window->height);
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (window->canvas), draw, window, NULL);
}

/*
新しいウィンドウを作成します。
*/
GtkWidget *viewer_document_window_new (GtkApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		APPLICATION_PROPERTY,    application,
		DEFAULT_HEIGHT_PROPERTY, DEFAULT_HEIGHT,
		DEFAULT_WIDTH_PROPERTY,  DEFAULT_WIDTH,
		MENUBAR_PROPERTY,        TRUE,
		TITLE_PROPERTY,          VIEWER_APPLICATION_TITLE,
		NULL);
}

static void viewer_window_settings_interface_init (ViewerWindowSettingsInterface *window)
{
	window->load = load;
	window->save = save;
}
