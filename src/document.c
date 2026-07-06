/* Copyright (C) 2026 Taichi Murakami. */
#include "viewer.h"
#define SUPER_CLASS viewer_document_window_parent_class

struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
	GFile *file;
};

static void dispose (GObject *object);
static void update (ViewerDocumentWindow *document);

static void viewer_document_window_class_init (ViewerDocumentWindowClass *self);
static void viewer_document_window_init       (ViewerDocumentWindow *self);

G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

static void dispose (GObject *object)
{
	g_clear_object (&VIEWER_DOCUMENT_WINDOW (object)->file);
	G_OBJECT_CLASS (SUPER_CLASS)->dispose (object);
}

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

GtkWidget *viewer_document_window_new (GApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		"application", application,
		NULL);
}

static void viewer_document_window_class_init (ViewerDocumentWindowClass *self)
{
	G_OBJECT_CLASS (self)->dispose = dispose;
}

GFile *viewer_document_window_get_file (ViewerDocumentWindow *self)
{
	return self->file;
}

static void viewer_document_window_init (ViewerDocumentWindow *self)
{
	gtk_window_set_default_size (GTK_WINDOW (self), 640, 480);
	update (self);
}

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
