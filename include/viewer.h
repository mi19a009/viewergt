/* Copyright (C) 2026 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define TEXT                            gettext
#define VIEWER_APPLICATION_ID           "com.github.mi19a009.PictureViewer"
#define VIEWER_LOGO_ICON_NAME           "viewer"
#define VIEWER_TITLE                    TEXT ("Picture Viewer")
#define VIEWER_TYPE_APPLICATION         viewer_application_get_type ()
#define VIEWER_TYPE_DOCUMENT_WINDOW     viewer_document_window_get_type ()

G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);

/* MAIN */
gboolean viewer_is_debug (void);

/* ABOUT DIALOG */
void viewer_about_dialog_run (GtkWindow *parent);

/* APPLICATION */
GApplication *viewer_application_new (const char *application_id);

/* DOCUMENT WINDOW */
GtkWidget *viewer_document_window_new (GApplication *application);
GFile *viewer_document_window_get_file (ViewerDocumentWindow *self);
void viewer_document_window_set_file (ViewerDocumentWindow *self, GFile *file);
