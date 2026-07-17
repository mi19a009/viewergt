/*
Copyright (C) 2026 Taichi Murakami.
ドキュメント ウィンドウ クラスを実装します。
*/
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "viewer.h"
#define APPLICATION_PROPERTY            "application"
#define DEFAULT_HEIGHT                  400
#define DEFAULT_HEIGHT_PROPERTY         "default-height"
#define DEFAULT_WIDTH                   600
#define DEFAULT_WIDTH_PROPERTY          "default-width"
#define MENUBAR_PROPERTY                "show-menubar"
#define TITLE_PROPERTY                  "title"

/* クラスのインスタンス */
struct _ViewerDocumentWindow
{
	GtkApplicationWindow parent_instance;
};

static void viewer_document_window_class_init (ViewerDocumentWindowClass *class);
static void viewer_document_window_init       (ViewerDocumentWindow *window);

/*
ドキュメント ウィンドウを表します。
ユーザーが開いた画像を格納します。
描画領域に画像を表示します。
*/
G_DEFINE_FINAL_TYPE (ViewerDocumentWindow, viewer_document_window, GTK_TYPE_APPLICATION_WINDOW);

/*
クラスを初期化します。
*/
static void viewer_document_window_class_init (ViewerDocumentWindowClass *class)
{
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_document_window_init (ViewerDocumentWindow *window)
{
}

/*
新しいウィンドウを作成します。
*/
GtkWidget *viewer_document_window_new (GtkApplication *application)
{
	return g_object_new (VIEWER_TYPE_DOCUMENT_WINDOW,
		APPLICATION_PROPERTY,    application,
		DEFAULT_HEIGHT_PROPERTY, DEFAULT_HEIGHT,
		DEFAULT_WIDTH_PROPERTY,  DEFAULT_WIDTH,
		MENUBAR_PROPERTY,        TRUE,
		TITLE_PROPERTY,          VIEWER_APPLICATION_TITLE,
		NULL);
}
