/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define COPYRIGHT               "Copyright © 2026 Taichi Murakami. All rights reserved."
#define WEBSITE                 "https://github.com/mi19a009"

static const char *AUTHORS [] = { "Taichi Murakami", NULL };

/*
バージョン情報を表示します。
*/
void viewer_about_dialog_run (GtkWindow *parent)
{
	GtkWidget *dialog;
	dialog = gtk_about_dialog_new ();
	gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), AUTHORS);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), COPYRIGHT);
	gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (dialog), GTK_LICENSE_MIT_X11);
	gtk_about_dialog_set_logo_icon_name (GTK_ABOUT_DIALOG (dialog), VIEWER_LOGO_ICON_NAME);
	gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), VIEWER_TITLE);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), WEBSITE);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
