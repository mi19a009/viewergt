/*
Copyright (C) 2026 Taichi Murakami.
ドキュメント ウィンドウ クラスを実装します。
*/
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define APPLICATION_PROPERTY            "application"
#define DEFAULT_HEIGHT                  400
#define DEFAULT_HEIGHT_PROPERTY         "default-height"
#define DEFAULT_WIDTH                   600
#define DEFAULT_WIDTH_PROPERTY          "default-width"
#define MENUBAR_PROPERTY                "show-menubar"
#define SUPER_CLASS                     viewer_document_window_parent_class
#define TITLE_PROPERTY                  "title"
#define WIDGET_TEMPLATE_NAME            (VIEWER_APPLICATION_PATH "/gtk/document.ui")

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GtkWidget *canvas;
};

static void dispose                           (GObject *object);
static void callback_draw                     (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data);
static void viewer_document_window_class_init (ViewerDocumentWindowClass *this_class);
static void viewer_document_window_init       (ViewerDocumentWindow *window);

/*
ドキュメント ウィンドウを表します。
ユーザーが開いた画像を格納します。
描画領域に画像を表示します。
*/
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/*
クラスのインスタンスを破棄します。
*/
static void dispose (GObject *object)
{
	gtk_widget_dispose_template (GTK_WIDGET (object), VIEWER_TYPE_DOCUMENT_WINDOW);
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/*
領域を描画します。
*/
static void callback_draw (GtkDrawingArea *area, cairo_t *cairo, int width, int height, gpointer user_data)
{
	cairo_set_source_rgb (cairo, 0.125, 0.25, 0.5);
	cairo_rectangle (cairo, 0, 0, width, height);
	cairo_fill (cairo);
}

/*
クラスを初期化します。
*/
static void viewer_document_window_class_init (ViewerDocumentWindowClass *this_class)
{
	G_OBJECT_CLASS (this_class)->dispose = dispose;
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (this_class), WIDGET_TEMPLATE_NAME);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (this_class), ViewerDocumentWindow, canvas);
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *window)
{
	gtk_widget_init_template (GTK_WIDGET (window));
	gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (window->canvas), callback_draw, window, NULL);
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
