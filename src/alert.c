/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>

/*
指定したエラーを説明する文字列を表示します。
*/
void viewer_alert_dialog_run (GtkWindow *parent, const GError *error)
{
	GtkWidget *widget;
	widget = gtk_message_dialog_new (parent, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", error->message);
	gtk_dialog_run (GTK_DIALOG (widget));
	gtk_widget_destroy (widget);
}
