/*
Copyright (C) 2026 Taichi Murakami.
アプリケーション クラスを実装します。
*/
#include <gtk/gtk.h>
#include "viewer.h"
#define APPLICATION_FLAGS               G_APPLICATION_HANDLES_OPEN
#define APPLICATION_FLAGS_PROPERTY      "flags"
#define APPLICATION_ID_PROPERTY         "application-id"
#define CLOSE_ACTION_DETAILED_NAME      "window.close"
#define DEBUG_OPTION_LONG_NAME          "debug"
#define DEBUG_OPTION_SHORT_NAME         0
#define DEBUG_OPTION_FLAGS              G_OPTION_FLAG_NONE
#define DEBUG_OPTION_ARG                G_OPTION_ARG_NONE
#define DEBUG_OPTION_ARG_DATA           ((gpointer) offsetof (ViewerApplication, debug))
#define DEBUG_OPTION_DESCRIPTION        "Enable debug."
#define DEBUG_OPTION_ARG_DESCRIPTION    NULL
#define HELP_ACTION_DETAILED_NAME       "win.show-help-overlay"
#define NEW_ACTION_ACTIVATE             callback_new
#define NEW_ACTION_DETAILED_NAME        "app.new"
#define NEW_ACTION_NAME                 "new"
#define OPTION_ENTRIES_MAX              G_N_ELEMENTS (OPTION_ENTRIES)
#define SUPER_CLASS                     viewer_application_parent_class
#define ACCEL_ENTRY(ACTION)             { ACTION ##_DETAILED_NAME, ACTION ##_ACCELS }
#define ACTION_ENTRY(ACTION)            { ACTION ##_NAME, ACTION ##_ACTIVATE }
#define DEFINE_ACCELS(ACCELS, ...)      static const char *ACCELS [] = { __VA_ARGS__, NULL }
#define OPTION_ENTRY(OPTION)            { OPTION ##_LONG_NAME, OPTION ##_SHORT_NAME, OPTION ##_FLAGS, OPTION ##_ARG, OPTION ##_ARG_DATA, OPTION ##_DESCRIPTION, OPTION ##_ARG_DESCRIPTION }

/* キーボード ショートカット */
typedef struct _ViewerApplicationAccelEntry
{
	const char *detailed_action_name;
	const char *const *accels;
} ViewerApplicationAccelEntry;

/* クラスのインスタンス */
struct _ViewerApplication
{
	GtkApplication parent_instance;
	gboolean debug;
};

static void       accel                         (GtkApplication *app);
static void       activate                      (GApplication *app);
static void       constructed                   (GObject *object);
static GtkWidget *create                        (ViewerApplication *app);
static void       open                          (GApplication *app, GFile **files, int n_files, const char *hint);
static void       option                        (ViewerApplication *app);
static void       startup                       (GApplication *app);
static void       callback_new                  (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       viewer_application_class_init (ViewerApplicationClass *this_class);
static void       viewer_application_init       (ViewerApplication *app);

/*
GTK アプリケーションを表します。
ユーザーが選択した印刷設定を格納します。
ドキュメント ウィンドウを作成して表示します。
*/
G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
DEFINE_ACCELS (CLOSE_ACTION_ACCELS, "<Ctrl>q");
DEFINE_ACCELS (HELP_ACTION_ACCELS, "<Ctrl>F1", "<Ctrl>question", "<Ctrl>slash");
DEFINE_ACCELS (NEW_ACTION_ACCELS, "<Ctrl>n");

/* キーボード ショートカット */
static const ViewerApplicationAccelEntry ACCEL_ENTRIES [] =
{
	ACCEL_ENTRY (CLOSE_ACTION),
	ACCEL_ENTRY (HELP_ACTION),
	ACCEL_ENTRY (NEW_ACTION),
};

/* メニュー項目のアクション */
static const GActionEntry ACTION_ENTRIES [] =
{
	ACTION_ENTRY (NEW_ACTION),
};

/* コマンド ライン オプション */
static const GOptionEntry OPTION_ENTRIES [] =
{
	OPTION_ENTRY (DEBUG_OPTION),
};

/*
キーボード ショートカットを設定します。
*/
static void accel (GtkApplication *app)
{
	const ViewerApplicationAccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (app, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

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
コマンド ライン オプションを利用可能にします。
*/
static void constructed (GObject *object)
{
	G_OBJECT_CLASS (SUPER_CLASS)->constructed (object);
	option (VIEWER_APPLICATION (object));
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
コマンド ライン オプションを利用可能にします。
*/
static void option (ViewerApplication *app)
{
	GOptionEntry entries [OPTION_ENTRIES_MAX + 1], *p;
	int n;
	memcpy (entries, OPTION_ENTRIES, sizeof OPTION_ENTRIES);
	memset (entries + OPTION_ENTRIES_MAX, 0, sizeof (GOptionEntry));
	p = entries;

	for (n = 0; n < OPTION_ENTRIES_MAX; n++)
	{
		p->arg_data = (char *) app + (size_t) p->arg_data;
		p++;
	}

	g_application_add_main_option_entries (G_APPLICATION (app), entries);
}

/*
メニュー項目のアクションを登録します。
キーボード ショートカットを登録します。
*/
static void startup (GApplication *app)
{
	G_APPLICATION_CLASS (SUPER_CLASS)->startup (app);
	gtk_window_set_default_icon_name (VIEWER_APPLICATION_ICON_NAME);
	g_action_map_add_action_entries (G_ACTION_MAP (app), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), app);
	accel (GTK_APPLICATION (app));
}

/*
新しいウィンドウを表示します。
*/
static void callback_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window;
	window = create (VIEWER_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (window));
}

/*
クラスを初期化します。
*/
static void viewer_application_class_init (ViewerApplicationClass *this_class)
{
	G_OBJECT_CLASS (this_class)->constructed = constructed;
	G_APPLICATION_CLASS (this_class)->activate = activate;
	G_APPLICATION_CLASS (this_class)->open = open;
	G_APPLICATION_CLASS (this_class)->startup = startup;
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
