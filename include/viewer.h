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
#define VIEWER_TYPE_DOCUMENT            (viewer_document_get_type        ())
#define VIEWER_TYPE_DOCUMENT_WINDOW     (viewer_document_window_get_type ())
#define VIEWER_TYPE_PRINT_OPERATION     (viewer_print_operation_get_type ())
#define VIEWER_TYPE_WINDOW_SETTINGS     (viewer_window_settings_get_type ())

typedef struct _ViewerDocument       ViewerDocument;
typedef struct _ViewerWindowSettings ViewerWindowSettings;

struct _ViewerDocumentInterface
{
	GTypeInterface g_iface;
	GFile     *(*get_file)  (ViewerDocument *self);
	GdkPixbuf *(*get_image) (ViewerDocument *self);
	gboolean   (*load)      (ViewerDocument *self, GFile *file, GError **error);
};

struct _ViewerWindowSettingsInterface
{
	GTypeInterface g_iface;
	void (* load) (ViewerWindowSettings *self, GSettings *settings);
	void (* save) (ViewerWindowSettings *self, GSettings *settings);
};

G_DECLARE_FINAL_TYPE (ViewerApplication,    viewer_application,     VIEWER, APPLICATION,     GtkApplication);
G_DECLARE_INTERFACE  (ViewerDocument,       viewer_document,        VIEWER, DOCUMENT,        GObject);
G_DECLARE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, VIEWER, DOCUMENT_WINDOW, GtkApplicationWindow);
G_DECLARE_FINAL_TYPE (ViewerPrintOperation, viewer_print_operation, VIEWER, PRINT_OPERATION, GtkPrintOperation);
G_DECLARE_INTERFACE  (ViewerWindowSettings, viewer_window_settings, VIEWER, WINDOW_SETTINGS, GtkWindow);

void viewer_init_locale (void);
void viewer_show_error  (GtkWindow *parent, const GError *error);

GtkWidget         *viewer_about_dialog_new    (void);
GApplication      *viewer_application_new     (const char *application_id);
GtkWidget         *viewer_document_window_new (GtkApplication *application);
GtkPrintOperation *viewer_print_operation_new (void);

GtkPrintSettings *viewer_application_get_print_settings (ViewerApplication *application);
void              viewer_application_set_print_settings (ViewerApplication *application, GtkPrintSettings *settings);

GFile     *viewer_document_get_file  (ViewerDocument *document);
GdkPixbuf *viewer_document_get_image (ViewerDocument *document);
gboolean   viewer_document_load      (ViewerDocument *document, GFile *file, GError **error);

GdkPixbuf *viewer_print_operation_get_document (ViewerPrintOperation *operation);
void       viewer_print_operation_set_document (ViewerPrintOperation *operation, GdkPixbuf *document);

void viewer_window_settings_load (ViewerWindowSettings *window, GSettings *settings);
void viewer_window_settings_save (ViewerWindowSettings *window, GSettings *settings);
