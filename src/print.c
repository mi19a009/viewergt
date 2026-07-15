/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define N_PAGES                 1
#define SIGNAL_BEGIN_PRINT      "begin-print"
#define SIGNAL_DRAW_PAGE        "draw-page"
#define SUPER_CLASS             viewer_print_operation_parent_class

/* クラスのインスタンス */
struct _ViewerPrintOperation
{
	GtkPrintOperation parent_instance;
	GdkPixbuf *pixbuf;
};

static void begin_print                       (GtkPrintOperation *operation, GtkPrintContext *context);
static void dispose                           (GObject *object);
static void dispose_properties                (ViewerPrintOperation *operation);
static void draw_page                         (GtkPrintOperation *operation, GtkPrintContext *context, int page);
static void get_property                      (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void set_property                      (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void viewer_print_operation_class_init (ViewerPrintOperationClass *operation_class);
static void viewer_print_operation_init       (ViewerPrintOperation *operation);

/* Viewer Print Operation クラス */
G_DEFINE_FINAL_TYPE (ViewerPrintOperation, viewer_print_operation, GTK_TYPE_PRINT_OPERATION);

/*
印刷を開始する場合に呼び出されます。
ページ数を設定します。
*/
static void begin_print (GtkPrintOperation *operation, GtkPrintContext *context)
{
	gtk_print_operation_set_n_pages (operation, N_PAGES);
}

/*
オブジェクトが破棄される場合に呼び出されます。
プロパティを破棄します。
*/
static void dispose (GObject *object)
{
	dispose_properties (VIEWER_PRINT_OPERATION (object));
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

/*
プロパティを破棄します。
*/
static void dispose_properties (ViewerPrintOperation *operation)
{
	g_clear_object (&operation->pixbuf);
}

/*
現在のドキュメントを印刷します。
*/
static void draw_page (GtkPrintOperation *operation, GtkPrintContext *context, int page)
{
	GdkPixbuf *pixbuf;
	cairo_t *cairo;
	double src_aspect, dest_width, dest_height, dest_aspect, scale, x, y;
	int src_width, src_height;
	pixbuf = VIEWER_PRINT_OPERATION (operation)->pixbuf;

	if (pixbuf)
	{
		dest_width = gtk_print_context_get_width (context);
		dest_height = gtk_print_context_get_height (context);
		src_width = gdk_pixbuf_get_width (pixbuf);
		src_height = gdk_pixbuf_get_height (pixbuf);
		dest_aspect = dest_width / dest_height;
		src_aspect = src_width / (double) src_height;

		if (src_aspect < dest_aspect)
		{
			scale = dest_height / src_height;
			x = ((dest_width / scale) - src_width) / 2;
			y = 0;
		}
		else
		{
			scale = dest_width / src_width;
			x = 0;
			y = ((dest_height / scale) - src_height) / 2;
		}

		cairo = gtk_print_context_get_cairo_context (context);
		cairo_scale (cairo, scale, scale);
		gdk_cairo_set_source_pixbuf (cairo, pixbuf, x, y);
		cairo_paint (cairo);
	}
}

/*
プロパティを取得します。
*/
static void get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

/*
プロパティを設定します。
*/
static void set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

/*
クラスを初期化します。
*/
static void viewer_print_operation_class_init (ViewerPrintOperationClass *operation_class)
{
	G_OBJECT_CLASS (operation_class)->dispose = dispose;
	G_OBJECT_CLASS (operation_class)->get_property = get_property;
	G_OBJECT_CLASS (operation_class)->set_property = set_property;
	GTK_PRINT_OPERATION_CLASS (operation_class)->begin_print = begin_print;
	GTK_PRINT_OPERATION_CLASS (operation_class)->draw_page = draw_page;
}

/*
印刷するドキュメントを取得します。
*/
GdkPixbuf *viewer_print_operation_get_pixbuf (ViewerPrintOperation *operation)
{
	GdkPixbuf *pixbuf;

	if (operation->pixbuf)
	{
		pixbuf = g_object_ref (operation->pixbuf);
	}
	else
	{
		pixbuf = NULL;
	}

	return pixbuf;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_print_operation_init (ViewerPrintOperation *operation)
{
}

/*
クラスのインスタンスを作成します。
*/
GtkPrintOperation *viewer_print_operation_new (void)
{
	return g_object_new (VIEWER_TYPE_PRINT_OPERATION, NULL);
}

/*
印刷するドキュメントを設定します。
*/
void viewer_print_operation_set_pixbuf (ViewerPrintOperation *operation, GdkPixbuf *pixbuf)
{
	if (operation->pixbuf != pixbuf)
	{
		if (operation->pixbuf)
		{
			g_object_unref (operation->pixbuf);
		}
		if (pixbuf)
		{
			operation->pixbuf = g_object_ref (pixbuf);
		}
		else
		{
			operation->pixbuf = NULL;
		}
	}
}
