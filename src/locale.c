/*
Copyright (C) 2026 Taichi Murakami.
メッセージ カタログの使用方法を提供します。
*/
#include <glib/gi18n.h>
#include <locale.h>

/*
メッセージ カタログへのパスを指定します。
*/
void viewer_init_locale (void)
{
	GError *error;
	char *path, *dir, *base;
	setlocale (LC_ALL, "");
	error = NULL;
	path = g_file_read_link ("/proc/self/exe", &error);

	if (path)
	{
		dir = g_path_get_dirname (path);
		base = g_path_get_basename (path);
		bindtextdomain (base, dir);
		bind_textdomain_codeset (base, "UTF-8");
		textdomain (base);
		g_free (base);
		g_free (dir);
		g_free (path);
	}
	if (error)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "%d. %s", error->code, error->message);
		g_error_free (error);
	}
}
