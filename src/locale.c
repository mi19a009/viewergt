/* Copyright (C) 2026 Taichi Murakami. */
#include <glib/gi18n.h>
#include <locale.h>
#include "viewer.h"
#define LOCALE_CODESET          "UTF-8"
#define LOCALE_LINK             "/proc/self/exe"
#define LOCALE_NAME             ""

void viewer_init_locale (void)
{
	GError *error;
	char *path, *dir, *base;
	setlocale (LC_ALL, LOCALE_NAME);
	error = NULL;
	path = g_file_read_link (LOCALE_LINK, &error);

	if (path)
	{
		dir = g_path_get_dirname (path);
		base = g_path_get_basename (path);
		bindtextdomain (base, dir);
		bind_textdomain_codeset (base, LOCALE_CODESET);
		textdomain (base);
		g_free (base);
		g_free (dir);
		g_free (path);
	}
	if (error)
	{
		viewer_show_error (NULL, error);
		g_error_free (error);
	}
}
