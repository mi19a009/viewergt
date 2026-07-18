/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define OPERATION_PAGES 1

struct _ViewerPrintOperation
{
	GtkPrintOperation parent_instance;
	GdkPixbuf        *document;
};

static void begin_print        (GtkPrintOperation *operation, GtkPrintContext *context);
static void calc_aspect        (GtkPrintContext *dest, GdkPixbuf *src, double *x, double *y, double *scale);
static void dispose            (GObject *object);
static void dispose_properties (ViewerPrintOperation *operation);
static void draw               (ViewerPrintOperation *operation, GtkPrintContext *context);
static void draw_page          (GtkPrintOperation *operation, GtkPrintContext *context, int page);
static void get_property       (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void set_property       (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void g_object_class_init               (GObjectClass *object);
static void gtk_print_operation_class_init    (GtkPrintOperationClass *operation);
static void viewer_print_operation_class_init (ViewerPrintOperationClass *operation);
static void viewer_print_operation_init       (ViewerPrintOperation *operation);

G_DEFINE_FINAL_TYPE (ViewerPrintOperation, viewer_print_operation, GTK_TYPE_PRINT_OPERATION);

static void begin_print (GtkPrintOperation *operation, GtkPrintContext *context)
{
	gtk_print_operation_set_n_pages (operation, OPERATION_PAGES);
}

static void calc_aspect (GtkPrintContext *dest, GdkPixbuf *src, double *x, double *y, double *z)
{
	double dest_width, dest_height, dest_aspect, src_width, src_height, src_aspect, scale;
	dest_width = gtk_print_context_get_width (dest);
	dest_height = gtk_print_context_get_height (dest);
	src_width = gdk_pixbuf_get_width (src);
	src_height = gdk_pixbuf_get_height (src);
	dest_aspect = dest_width / dest_height;
	src_aspect = src_width / src_height;

	if (src_aspect < dest_aspect)
	{
		scale = dest_height / src_height;
		*x = ((dest_width / scale) - src_width) / 2;
		*y = 0;
		*z = scale;
	}
	else
	{
		scale = dest_width / src_width;
		*x = 0;
		*y = ((dest_height / scale) - src_height) / 2;
		*z = scale;
	}
}

static void dispose (GObject *object)
{
	dispose_properties (VIEWER_PRINT_OPERATION (object));
	G_OBJECT_CLASS (viewer_print_operation_parent_class)->dispose (object);
}

static void dispose_properties (ViewerPrintOperation *operation)
{
	g_clear_object (&operation->document);
}

static void draw (ViewerPrintOperation *operation, GtkPrintContext *context)
{
	cairo_t *cairo;
	double x, y, scale;

	if (operation->document)
	{
		calc_aspect (context, operation->document, &x, &y, &scale);
		cairo = gtk_print_context_get_cairo_context (context);
		cairo_scale (cairo, scale, scale);
		gdk_cairo_set_source_pixbuf (cairo, operation->document, x, y);
		cairo_paint (cairo);
	}
}

static void draw_page (GtkPrintOperation *operation, GtkPrintContext *context, int page)
{
	draw (VIEWER_PRINT_OPERATION (operation), context);
}

static void get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void g_object_class_init (GObjectClass *object)
{
	object->dispose = dispose;
	object->get_property = get_property;
	object->set_property = set_property;
}

static void gtk_print_operation_class_init (GtkPrintOperationClass *operation)
{
	operation->begin_print = begin_print;
	operation->draw_page = draw_page;
}

static void viewer_print_operation_class_init (ViewerPrintOperationClass *operation)
{
	g_object_class_init (G_OBJECT_CLASS (operation));
	gtk_print_operation_class_init (GTK_PRINT_OPERATION_CLASS (operation));
}

GdkPixbuf *viewer_print_operation_get_document (ViewerPrintOperation *operation)
{
	GdkPixbuf *document;

	if (operation->document)
	{
		document = g_object_ref (operation->document);
	}
	else
	{
		document = NULL;
	}

	return document;
}

static void viewer_print_operation_init (ViewerPrintOperation *operation)
{
}

GtkPrintOperation *viewer_print_operation_new (void)
{
	return g_object_new (VIEWER_TYPE_PRINT_OPERATION, NULL);
}

void viewer_print_operation_set_document (ViewerPrintOperation *operation, GdkPixbuf *document)
{
	if (operation->document != document)
	{
		if (operation->document)
		{
			g_object_unref (operation->document);
		}
		if (document)
		{
			operation->document = g_object_ref (document);
		}
		else
		{
			operation->document = NULL;
		}
	}
}
