/*
Copyright (C) 2026 Taichi Murakami.
アプリケーションのメイン エントリ ポイントを提供します。
*/
#include <gtk/gtk.h>
#include "viewer.h"

/*
GTK アプリケーションを実行します。
*/
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
