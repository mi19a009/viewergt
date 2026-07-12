/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <locale.h>
#include "viewer.h"

/*
メッセージ カタログへのパスを指定します。
*/
static void init (void)
{
	GError *error;
	char *path, *dirname, *basename;
	setlocale (LC_ALL, "");
	error = NULL;
	path = g_file_read_link ("/proc/self/exe", &error);

	if (path)
	{
		dirname = g_path_get_dirname (path);
		basename = g_path_get_basename (path);
		bindtextdomain (basename, dirname);
		bind_textdomain_codeset (basename, "UTF-8");
		textdomain (basename);
		g_free (basename);
		g_free (dirname);
		g_free (path);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%d. %s", error->code, error->message);
		g_error_free (error);
	}
}

/*
GTK アプリケーションを実行します。
*/
int main (int argc, char **argv)
{
	GApplication *application;
	int status;
	init ();
	application = viewer_application_new (VIEWER_APPLICATION_ID);
	viewer_application_add_main_option_entries (VIEWER_APPLICATION (application));
	status = g_application_run (application, argc, argv);
	g_object_unref (application);
	return status;
}
