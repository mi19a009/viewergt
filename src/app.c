/* Copyright (C) 2026 Taichi Murakami. */
#include "viewer.h"
#define SUPER_CLASS viewer_application_parent_class

struct _ViewerApplication
{
	GtkApplication parent_instance;
};

static void activate (GApplication *application);
static void open     (GApplication *application, GFile **files, int n_files, const char *hint);
static void startup  (GApplication *application);

static void viewer_application_class_init (ViewerApplicationClass *self);
static void viewer_application_init       (ViewerApplication *self);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);

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

static void startup (GApplication *application)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->startup (application);
}

static void viewer_application_class_init (ViewerApplicationClass *self)
{
	G_APPLICATION_CLASS (self)->activate = activate;
	G_APPLICATION_CLASS (self)->open = open;
	G_APPLICATION_CLASS (self)->startup = startup;
}

static void viewer_application_init (ViewerApplication *self)
{
}

GApplication *viewer_application_new (const char *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		"application-id", application_id,
		"flags", G_APPLICATION_HANDLES_OPEN,
		NULL);
}
