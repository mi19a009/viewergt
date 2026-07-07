/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"

/*
アプリケーションのメイン エントリ ポイントです。
*/
int main (int argc, char *argv [])
{
	GApplication *application;
	int status;
	application = viewer_application_new (VIEWER_APPLICATION_ID);
	status = g_application_run (application, argc, argv);
	g_object_unref (application);
	return status;
}
