/*
Copyright (C) 2026 Taichi Murakami.
アプリケーションの機能を宣言します。
*/
#include <gtk/gtk.h>
#define TEXT(QUOTE)                     (gettext (QUOTE))
#define VIEWER_APPLICATION_ID           "com.github.mi19a009.PictureViewer"
#define VIEWER_APPLICATION_PATH         "/com/github/mi19a009/PictureViewer"
#define VIEWER_APPLICATION_TITLE        TEXT ("Picture Viewer")
#define VIEWER_APPLICATION_ICON_NAME    "viewer"
#define VIEWER_TYPE_APPLICATION         (viewer_application_get_type     ())
#define VIEWER_TYPE_DOCUMENT_WINDOW     (viewer_document_window_get_type ())

/* Viewer モジュール */
G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);

/* Viewer 関数 */
void viewer_init_locale (void);

/* Viewer 型 */
GtkWidget    *viewer_about_dialog_new    (void);
GApplication *viewer_application_new     (const char *application_id);
GtkWidget    *viewer_document_window_new (GtkApplication *application);
