/* Copyright (C) 2026 Taichi Murakami. */
#pragma once
#include <gtk/gtk.h>
#define TEXT                            gettext
#define VIEWER_APPLICATION_ID           "com.github.mi19a009.PictureViewer"
#define VIEWER_LOGO_ICON_NAME           "viewer"
#define VIEWER_TITLE                    TEXT ("Picture Viewer")
#define VIEWER_WINDOW_SETTINGS_ID       VIEWER_APPLICATION_ID ".window"
#define VIEWER_TYPE_APPLICATION         (viewer_application_get_type ())
#define VIEWER_TYPE_DOCUMENT_WINDOW     (viewer_document_window_get_type ())
#define VIEWER_TYPE_PRINT_OPERATION     (viewer_print_operation_get_type ())
#define OBJECT_CLASS_INSTALL_PROPERTY(object, PROPERTY, PSPEC) g_object_class_install_property ((object), PROPERTY ##_ID, PSPEC (PROPERTY))
#define OPTION_ENTRY_COLUMN(OPTION) OPTION ##_LONG_NAME, OPTION ##_SHORT_NAME, OPTION ##_FLAGS, OPTION ##_ARG, OPTION ##_ARG_DATA, OPTION ##_DESCRIPTION, OPTION ##_ARG_DESCRIPTION
#define PARAM_SPEC_OBJECT(PROPERTY) g_param_spec_object (PROPERTY ##_NAME, PROPERTY ##_NICK, PROPERTY ##_BLURB, PROPERTY ##_OBJECT_TYPE, PROPERTY ##_FLAGS)
#ifdef NDEBUG
#define VIEWER_PRINT(format, ...)
#else
#define VIEWER_PRINT(format, ...) g_print ((format), __VA_ARGS__)
#endif

/* Viewer モジュール */
G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (ViewerPrintOperation, viewer_print_operation, VIEWER, PRINT_OPERATION, GtkPrintOperation);

/* Viewer About Dialog */
void viewer_about_dialog_run (GtkWindow *parent);

/* Viewer Alert Dialog */
void viewer_alert_dialog_run (GtkWindow *parent, const GError *error);

/* Viewer Application */
void              viewer_application_add_main_option_entries (ViewerApplication *application);
GtkPrintSettings *viewer_application_get_print_settings      (ViewerApplication *application);
GApplication     *viewer_application_new                     (const char *application_id);
void              viewer_application_set_print_settings      (ViewerApplication *application, GtkPrintSettings *settings);

/* Viewer Document Window */
GFile     *viewer_document_window_get_file      (ViewerDocumentWindow *window);
GdkPixbuf *viewer_document_window_get_pixbuf    (ViewerDocumentWindow *window);
void       viewer_document_window_load_settings (ViewerDocumentWindow *window, GSettings *settings);
GtkWidget *viewer_document_window_new           (GtkApplication *application);
void       viewer_document_window_save_settings (ViewerDocumentWindow *window, GSettings *settings);
void       viewer_document_window_set_file      (ViewerDocumentWindow *window, GFile *file);
void       viewer_document_window_set_pixbuf    (ViewerDocumentWindow *window, GdkPixbuf *pixbuf);

/* Viewer File Chooser Dialog */
GtkWidget *viewer_file_chooser_dialog_new (GtkWindow *parent);

/* Viewer Print Operation */
GdkPixbuf         *viewer_print_operation_get_pixbuf (ViewerPrintOperation *operation);
GtkPrintOperation *viewer_print_operation_new        (void);
void               viewer_print_operation_set_pixbuf (ViewerPrintOperation *operation, GdkPixbuf *pixbuf);
