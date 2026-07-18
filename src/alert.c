/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#define ERROR_FORMAT "%d. %s"

void viewer_show_error (GtkWindow *parent, const GError *error)
{
	GtkAlertDialog *dialog;

	if (parent)
	{
		dialog = gtk_alert_dialog_new (ERROR_FORMAT, error->code, error->message);
		gtk_alert_dialog_show (dialog, parent);
		g_object_unref (dialog);
	}
	else
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, ERROR_FORMAT, error->code, error->message);
	}
}
