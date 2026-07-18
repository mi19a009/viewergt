/* Copyright (C) 2026 Taichi Murakami. */
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
#define NEW_ACTION_ACTIVATE             activate_new
#define NEW_ACTION_DETAILED_NAME        "app.new"
#define NEW_ACTION_NAME                 "new"
#define OPTION_ENTRIES_MAX              G_N_ELEMENTS (OPTION_ENTRIES)
#define SIGNAL_DESTROY                  "destroy"
#define WINDOW_SETTINGS                 (VIEWER_APPLICATION_ID ".window")
#define ACCEL_ENTRY(ACTION)             { ACTION ##_DETAILED_NAME, ACTION ##_ACCELS }
#define ACTION_ENTRY(ACTION)            { ACTION ##_NAME, ACTION ##_ACTIVATE }
#define DEFINE_ACCELS(ACCELS, ...)      static const char *ACCELS [] = { __VA_ARGS__, NULL }
#define OPTION_ENTRY(OPTION)            { OPTION ##_LONG_NAME, OPTION ##_SHORT_NAME, OPTION ##_FLAGS, OPTION ##_ARG, OPTION ##_ARG_DATA, OPTION ##_DESCRIPTION, OPTION ##_ARG_DESCRIPTION }

typedef struct _ViewerApplicationAccelEntry
{
	const char *detailed_action_name;
	const char *const *accels;
} ViewerApplicationAccelEntry;

struct _ViewerApplication
{
	GtkApplication    parent_instance;
	GtkPrintSettings *print_settings;
	gboolean          debug;
};

static void       accelerate         (GtkApplication *application);
static void       activate           (GApplication *application);
static void       activate_new       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void       add_options        (ViewerApplication *application);
static void       constructed        (GObject *object);
static GtkWidget *create_window      (ViewerApplication *application);
static void       destroy_window     (GtkWidget *widget, gpointer user_data);
static void       dispose            (GObject *object);
static void       dispose_properties (ViewerApplication *application);
static void       get_property       (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void       load_document      (ViewerDocument *document, GFile *file, GtkWindow *parent);
static void       load_window        (ViewerWindowSettings *window);
static void       open               (GApplication *application, GFile **files, int n_files, const char *hint);
static void       save_window        (ViewerWindowSettings *window);
static void       set_property       (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void       startup            (GApplication *application);

static void g_application_class_init      (GApplicationClass *application);
static void g_object_class_init           (GObjectClass *object);
static void viewer_application_class_init (ViewerApplicationClass *application);
static void viewer_application_init       (ViewerApplication *application);

G_DEFINE_FINAL_TYPE (ViewerApplication, viewer_application, GTK_TYPE_APPLICATION);
DEFINE_ACCELS (CLOSE_ACTION_ACCELS, "<Ctrl>q");
DEFINE_ACCELS (HELP_ACTION_ACCELS, "<Ctrl>F1", "<Ctrl>question", "<Ctrl>slash");
DEFINE_ACCELS (NEW_ACTION_ACCELS, "<Ctrl>n");

static const ViewerApplicationAccelEntry ACCEL_ENTRIES [] =
{
	ACCEL_ENTRY (CLOSE_ACTION),
	ACCEL_ENTRY (HELP_ACTION),
	ACCEL_ENTRY (NEW_ACTION),
};

static const GActionEntry ACTION_ENTRIES [] =
{
	ACTION_ENTRY (NEW_ACTION),
};

static const GOptionEntry OPTION_ENTRIES [] =
{
	OPTION_ENTRY (DEBUG_OPTION),
};

static void accelerate (GtkApplication *application)
{
	const ViewerApplicationAccelEntry *entries;
	int n;
	entries = ACCEL_ENTRIES;

	for (n = 0; n < G_N_ELEMENTS (ACCEL_ENTRIES); n++)
	{
		gtk_application_set_accels_for_action (application, entries->detailed_action_name, entries->accels);
		entries++;
	}
}

static void activate (GApplication *application)
{
	GtkWindow *window;
	window = gtk_application_get_active_window (GTK_APPLICATION (application));

	if (!window)
	{
		window = GTK_WINDOW (create_window (VIEWER_APPLICATION (application)));
	}

	gtk_window_present (window);
}

static void activate_new (GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GtkWidget *window;
	window = create_window (VIEWER_APPLICATION (user_data));
	gtk_window_present (GTK_WINDOW (window));
}

static void add_options (ViewerApplication *application)
{
	GOptionEntry entries [OPTION_ENTRIES_MAX + 1], *p;
	int n;
	memcpy (entries, OPTION_ENTRIES, sizeof OPTION_ENTRIES);
	memset (entries + OPTION_ENTRIES_MAX, 0, sizeof (GOptionEntry));
	p = entries;

	for (n = 0; n < OPTION_ENTRIES_MAX; n++)
	{
		p->arg_data = (char *) application + (size_t) p->arg_data;
		p++;
	}

	g_application_add_main_option_entries (G_APPLICATION (application), entries);
}

static void constructed (GObject *object)
{
	G_OBJECT_CLASS (viewer_application_parent_class)->constructed (object);
	add_options (VIEWER_APPLICATION (object));
}

static GtkWidget *create_window (ViewerApplication *application)
{
	GtkWidget *window;
	window = viewer_document_window_new (GTK_APPLICATION (application));

	if (!application->debug)
	{
		g_signal_connect (window, SIGNAL_DESTROY, G_CALLBACK (destroy_window), application);
		load_window (VIEWER_WINDOW_SETTINGS (window));
	}

	return window;
}

static void destroy_window (GtkWidget *widget, gpointer user_data)
{
	save_window (VIEWER_WINDOW_SETTINGS (widget));
}

static void dispose (GObject *object)
{
	dispose_properties (VIEWER_APPLICATION (object));
	G_OBJECT_CLASS (viewer_application_parent_class)->dispose (object);
}

static void dispose_properties (ViewerApplication *application)
{
	g_clear_object (&application->print_settings);
}

static void get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void load_document (ViewerDocument *document, GFile *file, GtkWindow *parent)
{
	GError *error;
	error = NULL;
	viewer_document_load (document, file, &error);

	if (error)
	{
		viewer_show_error (parent, error);
		g_error_free (error);
	}
}

static void load_window (ViewerWindowSettings *window)
{
	GSettings *settings;
	settings = g_settings_new (WINDOW_SETTINGS);
	viewer_window_settings_load (window, settings);
	g_object_unref (settings);
}

static void open (GApplication *application, GFile **files, int n_files, const char *hint)
{
	GtkWidget *window;
	ViewerDocument *document;
	int n;
	n = 0;

	if (n_files > 0)
	{
		window = GTK_WIDGET (gtk_application_get_active_window (GTK_APPLICATION (application)));

		if (VIEWER_IS_DOCUMENT (window))
		{
			document = VIEWER_DOCUMENT (window);

			if (viewer_document_get_file (document))
			{
				load_document (document, *(files++), GTK_WINDOW (window));
				gtk_window_present (GTK_WINDOW (window));
				n++;
			}
		}
		while (n < n_files)
		{
			window = create_window (VIEWER_APPLICATION (application));
			load_document (VIEWER_DOCUMENT (window), *(files++), GTK_WINDOW (window));
			gtk_window_present (GTK_WINDOW (window));
			n++;
		}
	}
}

static void save_window (ViewerWindowSettings *window)
{
	GSettings *settings;
	settings = g_settings_new (WINDOW_SETTINGS);
	viewer_window_settings_save (window, settings);
	g_object_unref (settings);
}

static void set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}

static void startup (GApplication *application)
{
	G_APPLICATION_CLASS (viewer_application_parent_class)->startup (application);
	gtk_window_set_default_icon_name (VIEWER_APPLICATION_ICON_NAME);
	g_action_map_add_action_entries (G_ACTION_MAP (application), ACTION_ENTRIES, G_N_ELEMENTS (ACTION_ENTRIES), application);
	accelerate (GTK_APPLICATION (application));
}

static void g_application_class_init (GApplicationClass *application)
{
	application->activate = activate;
	application->open = open;
	application->startup = startup;
}

static void g_object_class_init (GObjectClass *object)
{
	object->constructed = constructed;
	object->dispose = dispose;
	object->get_property = get_property;
	object->set_property = set_property;
}

static void viewer_application_class_init (ViewerApplicationClass *application)
{
	g_object_class_init (G_OBJECT_CLASS (application));
	g_application_class_init (G_APPLICATION_CLASS (application));
}

static void viewer_application_init (ViewerApplication *application)
{
}

GApplication *viewer_application_new (const char *application_id)
{
	return g_object_new (VIEWER_TYPE_APPLICATION,
		APPLICATION_ID_PROPERTY,    application_id,
		APPLICATION_FLAGS_PROPERTY, APPLICATION_FLAGS,
		NULL);
}
