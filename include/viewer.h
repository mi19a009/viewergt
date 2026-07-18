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
#define VIEWER_TYPE_WINDOW_SETTINGS     (viewer_window_settings_get_type ())

typedef struct _ViewerWindowSettings ViewerWindowSettings;

struct _ViewerWindowSettingsInterface
{
	GTypeInterface g_iface;
	void (* load) (ViewerWindowSettings *self, GSettings *settings);
	void (* save) (ViewerWindowSettings *self, GSettings *settings);
};

/* Viewer モジュール */
G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);
G_DECLARE_INTERFACE  (ViewerWindowSettings, viewer_window_settings, VIEWER, WINDOW_SETTINGS, GtkWindow);

/* Viewer 関数 */
void viewer_init_locale (void);

/* Viewer 型 */
GtkWidget    *viewer_about_dialog_new    (void);
GApplication *viewer_application_new     (const char *application_id);
GtkWidget    *viewer_document_window_new (GtkApplication *application);

/* Viewer Window Settings 型 */
void viewer_window_settings_load (ViewerWindowSettings *window, GSettings *settings);
void viewer_window_settings_save (ViewerWindowSettings *window, GSettings *settings);
