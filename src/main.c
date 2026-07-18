/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"

int main (int argc, char **argv)
{
	GApplication *application;
	int result;
	viewer_init_locale ();
	application = viewer_application_new (VIEWER_APPLICATION_ID);
	result = g_application_run (application, argc, argv);
	g_object_unref (application);
	return result;
}
