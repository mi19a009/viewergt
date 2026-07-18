/* Copyright (C) 2026 Taichi Murakami. */
#include <gtk/gtk.h>
#include "viewer.h"
#define IMPLEMENT_ACTION(self, method)         INTERFACE *iface; iface = INTERFACE_FROM (self);        iface->method (self)
#define IMPLEMENT_ACTION_(self, method, ...)   INTERFACE *iface; iface = INTERFACE_FROM (self);        iface->method (self, __VA_ARGS__)
#define IMPLEMENT_FUNCTION(self, method)       INTERFACE *iface; iface = INTERFACE_FROM (self); return iface->method (self)
#define IMPLEMENT_FUNCTION_(self, method, ...) INTERFACE *iface; iface = INTERFACE_FROM (self); return iface->method (self, __VA_ARGS__)

static void viewer_document_default_init        (ViewerDocumentInterface *document);
static void viewer_window_settings_default_init (ViewerWindowSettingsInterface *window);

G_DEFINE_INTERFACE (ViewerDocument,       viewer_document,        G_TYPE_OBJECT);
G_DEFINE_INTERFACE (ViewerWindowSettings, viewer_window_settings, GTK_TYPE_WINDOW);

static void viewer_document_default_init (ViewerDocumentInterface *document)
{
}

static void viewer_window_settings_default_init (ViewerWindowSettingsInterface *window)
{
}

#undef INTERFACE
#undef INTERFACE_FROM
#define INTERFACE ViewerDocumentInterface
#define INTERFACE_FROM VIEWER_DOCUMENT_GET_IFACE

GFile *viewer_document_get_file (ViewerDocument *document)
{
	IMPLEMENT_FUNCTION (document, get_file);
}

GdkPixbuf *viewer_document_get_image (ViewerDocument *document)
{
	IMPLEMENT_FUNCTION (document, get_image);
}

gboolean viewer_document_load (ViewerDocument *document, GFile *file, GError **error)
{
	IMPLEMENT_FUNCTION_ (document, load, file, error);
}

#undef INTERFACE
#undef INTERFACE_FROM
#define INTERFACE ViewerWindowSettingsInterface
#define INTERFACE_FROM VIEWER_WINDOW_SETTINGS_GET_IFACE

void viewer_window_settings_load (ViewerWindowSettings *window, GSettings *settings)
{
	IMPLEMENT_ACTION_ (window, load, settings);
}

void viewer_window_settings_save (ViewerWindowSettings *window, GSettings *settings)
{
	IMPLEMENT_ACTION_ (window, save, settings);
}
