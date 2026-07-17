/*
Copyright (C) 2026 Taichi Murakami.
アプリケーション クラスを実装します。
*/
#include <gtk/gtk.h>
#include "viewer.h"
#define APPLICATION_FLAGS               G_APPLICATION_HANDLES_OPEN
#define APPLICATION_FLAGS_PROPERTY      "flags"
#define APPLICATION_ID_PROPERTY         "application-id"
#define SUPER_CLASS                     viewer_application_parent_class

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
};

static void       activate                      (GApplication *app);
static GtkWidget *create                        (ViewerApplication *app);
static void       open                          (GApplication *app, GFile **files, int n_files, const char *hint);
static void       startup                       (GApplication *app);
static void       viewer_application_class_init (ViewerApplicationClass *class);
static void       viewer_application_init       (ViewerApplication *app);

/*
GTK アプリケーションを表します。
ユーザーが選択した印刷設定を格納します。
ドキュメント ウィンドウを作成して表示します。
*/
G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);

/*
既存のウィンドウまたは新しいウィンドウを表示します。
*/
static void activate (GApplication *app)
{
	GtkWindow *window;
	window = gtk_application_get_active_window (GTK_APPLICATION (app));

	if (!window)
	{
		window = GTK_WINDOW (create (VIEWER_APPLICATION (app)));
	}

	gtk_window_present (window);
}

/*
新しいドキュメント ウィンドウを作成します。
*/
static GtkWidget *create (ViewerApplication *app)
{
	GtkWidget *window;
	window = viewer_document_window_new (GTK_APPLICATION (app));
	return window;
}

/*
開くファイルを指定して既存のウィンドウまたは新しいウィンドウを表示します。
*/
static void open (GApplication *app, GFile **files, int n_files, const char *hint)
{
}

/*
メニュー項目のアクションを登録します。
キーボード ショートカットを登録します。
*/
static void startup (GApplication *app)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->startup (app);
	gtk_window_set_default_icon_name (VIEWER_APPLICATION_ICON_NAME);
}

/*
クラスを初期化します。
*/
static void viewer_application_class_init (ViewerApplicationClass *class)
{
	G_APPLICATION_CLASS (class)->activate = activate;
	G_APPLICATION_CLASS (class)->open = open;
	G_APPLICATION_CLASS (class)->startup = startup;
}

/*
クラスのインスタンスを初期化します。
*/
static void viewer_application_init (ViewerApplication *app)
{
}

/*
新しいアプリケーションを作成します。
*/
GApplication *viewer_application_new (const char *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		APPLICATION_ID_PROPERTY,    application_id,
		APPLICATION_FLAGS_PROPERTY, APPLICATION_FLAGS,
		NULL);
}
