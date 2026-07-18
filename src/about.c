/*
Copyright (C) 2026 Taichi Murakami.
バージョン情報ダイアログを表示する方法を提供します。
*/
#include <gtk/gtk.h>
#include "viewer.h"
#define RESOURCE_NAME "dialog"
#define RESOURCE_PATH (VIEWER_APPLICATION_PATH "/gtk/about.ui")

/*
リソースからウィジェットを取得します。
*/
GtkWidget *viewer_about_dialog_new (void)
{
	GtkBuilder *builder;
	GObject *dialog;
	builder = gtk_builder_new_from_resource (RESOURCE_PATH);
	dialog = gtk_builder_get_object (builder, RESOURCE_NAME);
	g_object_unref (builder);
	return GTK_WIDGET (dialog);
}
