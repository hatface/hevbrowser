/* hevmainwindow.c
 * Heiher <admin@heiher.info>
 */

#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

#include "hevmainwindow.h"
#include "hevoutputwindow.h"
#include "hevconfig.h"

#define HEV_MAIN_WINDOW_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_MAIN_WINDOW, HevMainWindowPrivate))

typedef struct _HevMainWindowPrivate HevMainWindowPrivate;

struct _HevMainWindowPrivate
{
	GtkAccelGroup * accel_group;
	GObject * config;
	GtkWidget * output_window;
	GtkWidget * web_view;
	GtkToolItem * button_go_back;
	GtkToolItem * button_go_forward;
	GtkToolItem * button_reload;
	GtkToolItem * button_stop;
	GtkToolItem * button_home;
	GtkToolItem * button_execute;
	GtkWidget * entry;
	GtkWidget * label;
	GtkWidget * progress_bar;
};

enum
{
	ACCEL_F5,
	ACCEL_F6,
	ACCEL_F7,
	ACCEL_F8,
	ACCEL_ESC,
	LAST_SIGNAL
};

static guint hev_main_window_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(HevMainWindow, hev_main_window, GTK_TYPE_WINDOW);

static void hev_main_window_real_destroy(GtkWidget *widget)
{
	gtk_main_quit();
}

static void hev_main_window_entry_real_activate(GtkEntry * entry, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	GString * url = NULL;

	url = g_string_new("");
	const gchar * str = gtk_entry_get_text(entry);
	if(!g_regex_match_simple(".*://.*", str, 0, 0))
	{
		gchar * def_protocol = hev_config_get_def_protocol(HEV_CONFIG(priv->config));

		g_string_printf(url, "%s://%s", def_protocol, str);
		g_free(def_protocol);
	}
	else
	{
		g_string_printf(url, "%s", str);
	}
	webkit_web_view_open(WEBKIT_WEB_VIEW(priv->web_view), url->str);
	g_string_free(url, TRUE);
}

static void hev_main_window_web_view_real_title_changed(WebKitWebView * web_view, WebKitWebFrame * frame,
			gchar * title, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	GString * realtitle = NULL;

	realtitle = g_string_new("");
	g_string_printf(realtitle, "%s - %s", title, "HevBrowser");
	gtk_window_set_title(GTK_WINDOW(window), realtitle->str);
	g_string_free(realtitle, TRUE);
}

static void hev_main_window_web_view_real_load_committed(WebKitWebView * web_view, WebKitWebFrame * frame,
			gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	
	gtk_entry_set_text(GTK_ENTRY(priv->entry), webkit_web_frame_get_uri(frame));

	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_go_back),
				webkit_web_view_can_go_back(web_view));
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_go_forward),
				webkit_web_view_can_go_forward(web_view));
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_reload), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_execute), FALSE);
}

static void hev_main_window_web_view_real_load_started(WebKitWebView * web_view, WebKitWebFrame * frame,
			gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_stop), TRUE);
	gtk_widget_show(priv->progress_bar);
	gtk_label_set_text(GTK_LABEL(priv->label), "Loading ...");
}

static void hev_main_window_web_view_real_load_finished(WebKitWebView * web_view, WebKitWebFrame * frame,
			gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_stop), FALSE);
	gtk_widget_hide(priv->progress_bar);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->progress_bar), 0.0);
	gtk_label_set_text(GTK_LABEL(priv->label), "Done");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_execute), TRUE);
}

static void hev_main_window_web_view_real_load_progress_changed(WebKitWebView * web_view, gint pos,
			gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(priv->progress_bar),
				webkit_web_view_get_progress(web_view));
}

static WebKitWebView * hev_main_window_web_view_real_create_web_view(WebKitWebView * web_view, WebKitWebFrame * web_frame,
			gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	return web_view;
}

static void hev_main_window_button_open_file_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	GtkWidget * dialog = NULL;

	dialog = gtk_file_chooser_dialog_new("Open file",
				GTK_WINDOW(window),
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	if(GTK_RESPONSE_ACCEPT == gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		GString * url = g_string_new("");
		gchar * filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		g_string_printf(url, "file://%s", filename);
		webkit_web_view_open(WEBKIT_WEB_VIEW(priv->web_view), url->str);
		g_free(filename);
		g_string_free(url, TRUE);
	}
	gtk_widget_destroy(dialog);
}

static void hev_main_window_button_go_back_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	webkit_web_view_go_back(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_button_go_forward_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	webkit_web_view_go_forward(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_button_reload_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	webkit_web_view_reload(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_button_stop_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	webkit_web_view_stop_loading(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_button_home_real_clicked(GtkToolButton * button, gpointer data)
{
	gchar * home_url = NULL;
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	home_url = hev_config_get_home_url(HEV_CONFIG(priv->config));
	webkit_web_view_open(WEBKIT_WEB_VIEW(priv->web_view), home_url);
	g_free(home_url);
}

static void hev_main_window_button_execute_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	WebKitWebFrame * web_frame = webkit_web_view_get_main_frame(WEBKIT_WEB_VIEW(priv->web_view));
	gchar * js_path = NULL;
	gchar * js = NULL;

	/* exec internal js START */
	js_path = hev_config_get_inner_js_path(HEV_CONFIG(priv->config));
	if(!g_file_get_contents(js_path, &js, NULL, NULL))
	{
		GtkWidget * dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Load %s failed!", js_path);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	else
	{
		webkit_web_view_execute_script(WEBKIT_WEB_VIEW(priv->web_view), js);
	}
	g_free(js_path);
	/* exec internal js END */

	/* exec external js START */
	js_path = hev_config_get_js_path(HEV_CONFIG(priv->config));
	if(!g_file_get_contents(js_path, &js, NULL, NULL))
	{
		GtkWidget * dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Load %s failed!", js_path);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
	else
	{
		webkit_web_view_execute_script(WEBKIT_WEB_VIEW(priv->web_view), js);
	}
	g_free(js_path);
	/* exec external js END */

	/* call main START */
	{
		JSStringRef js_str = JSStringCreateWithUTF8CString("main();");
		JSGlobalContextRef js_ctx = webkit_web_frame_get_global_context(web_frame);

		JSValueRef js_val = JSEvaluateScript(js_ctx, js_str, NULL, NULL,	0, NULL);
		if(FALSE == JSValueIsString(js_ctx, js_val))
		{
			GtkWidget * dialog = gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"JS interface error!");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
		}
		else
		{
			JSStringRef js_ret = JSValueToStringCopy(js_ctx, js_val, NULL);
			if(NULL == js_ret)
			{
				GtkWidget * dialog = gtk_message_dialog_new(GTK_WINDOW(window),
							GTK_DIALOG_DESTROY_WITH_PARENT,
							GTK_MESSAGE_ERROR,
							GTK_BUTTONS_CLOSE,
							"Unknown error!");
				gtk_dialog_run(GTK_DIALOG(dialog));
				gtk_widget_destroy(dialog);
			}
			else
			{
				gsize len = JSStringGetMaximumUTF8CStringSize(js_ret);
				gchar * buf = g_malloc0(len);
				JSStringGetUTF8CString(js_ret, buf, len);
				hev_output_window_append_text(HEV_OUTPUT_WINDOW(priv->output_window), buf, -1);
				JSStringRelease(js_ret);
				g_free(buf);
			}
		}

		JSStringRelease(js_str);
	}
	/* call main END */
}

static void hev_main_window_button_show_output_win_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_window_present(GTK_WINDOW(priv->output_window));
}

static void hev_main_window_button_show_about_dlg_real_clicked(GtkToolButton * button, gpointer data)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(data);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);
	gchar * authors[] =
	{
		"Heiher <admin@heiher.info>",
		NULL
	};

	gtk_show_about_dialog(GTK_WINDOW(window),
				"program-name", "HevBrowser",
				"title", "About HevBrowser",
				"copyright", "Copyright © 2010 Heiher.info",
				"website", "http://git.heiher.info/hevbrowser.git",
				"website-label", "Code repository",
				"version", "0.0.1",
				"authors", authors,
				NULL);
}

static void hev_main_window_real_accel_f5(HevMainWindow * self)
{
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(self);

	webkit_web_view_reload(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_real_accel_f6(HevMainWindow * self)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(self);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_window_set_focus(GTK_WINDOW(self), priv->entry);
}

static void hev_main_window_real_accel_f7(HevMainWindow * self)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(self);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	if(gtk_widget_get_sensitive(GTK_WIDGET(priv->button_execute)))
	{
		g_signal_emit_by_name(G_OBJECT(priv->button_execute), "clicked", self);
	}
}

static void hev_main_window_real_accel_f8(HevMainWindow * self)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(self);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	gtk_window_present(GTK_WINDOW(priv->output_window));
}

static void hev_main_window_real_accel_esc(HevMainWindow * self)
{
	HevMainWindow * window = HEV_MAIN_WINDOW(self);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(window);

	webkit_web_view_stop_loading(WEBKIT_WEB_VIEW(priv->web_view));
}

static void hev_main_window_dispose(GObject * obj)
{
	HevMainWindow * self = HEV_MAIN_WINDOW(obj);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(self);

	if(GTK_IS_WINDOW(priv->output_window))
	{
		gtk_widget_destroy(priv->output_window);
		priv->output_window = NULL;
	}

	if(GTK_IS_ACCEL_GROUP(priv->accel_group))
	{
		g_object_unref(priv->accel_group);
		priv->accel_group = NULL;
	}

	if(HEV_IS_CONFIG(priv->config))
	{
		g_object_unref(priv->config);
		priv->config = NULL;
	}

	G_OBJECT_CLASS(hev_main_window_parent_class)->dispose(obj);
}

static void hev_main_window_finalize(GObject * obj)
{
	HevMainWindow * self = HEV_MAIN_WINDOW(obj);
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(self);

	G_OBJECT_CLASS(hev_main_window_parent_class)->finalize(obj);
}

static void hev_main_window_class_init(HevMainWindowClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	obj_class->dispose = hev_main_window_dispose;
	obj_class->finalize = hev_main_window_finalize;

	klass->accel_f5 = hev_main_window_real_accel_f5;
	klass->accel_f6 = hev_main_window_real_accel_f6;
	klass->accel_f7 = hev_main_window_real_accel_f7;
	klass->accel_f8 = hev_main_window_real_accel_f8;
	klass->accel_esc = hev_main_window_real_accel_esc;

	widget_class->destroy = hev_main_window_real_destroy;

	g_type_class_add_private(klass, sizeof(HevMainWindowPrivate));

	hev_main_window_signals[ACCEL_F5] = g_signal_new("accel_f5",
				G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET(HevMainWindowClass, accel_f5),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	hev_main_window_signals[ACCEL_F6] = g_signal_new("accel_f6",
				G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET(HevMainWindowClass, accel_f6),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	hev_main_window_signals[ACCEL_F7] = g_signal_new("accel_f7",
				G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET(HevMainWindowClass, accel_f7),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	hev_main_window_signals[ACCEL_F8] = g_signal_new("accel_f8",
				G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET(HevMainWindowClass, accel_f8),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);

	hev_main_window_signals[ACCEL_ESC] = g_signal_new("accel_esc",
				G_TYPE_FROM_CLASS(klass),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET(HevMainWindowClass, accel_esc),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);
}

static void hev_main_window_init(HevMainWindow * self)
{
	gchar * home_url = NULL;
	GtkWidget * vbox = NULL;
	GtkWidget * hbox = NULL;
	GtkWidget * toolbar = NULL;
	GtkToolItem * button = NULL;
	GtkWidget * label = NULL;
	GtkWidget * scrolled_window = NULL;
	HevMainWindowPrivate * priv = HEV_MAIN_WINDOW_GET_PRIVATE(self);

	gtk_window_set_title(GTK_WINDOW(self), "HevBrowser");
	gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER);
	gtk_window_maximize(GTK_WINDOW(self));
	gtk_widget_set_size_request(GTK_WIDGET(self), 800, 600);
	gtk_container_set_border_width(GTK_CONTAINER(self), 2);

	priv->config = hev_config_new();
	priv->accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(self), priv->accel_group);

	gtk_widget_add_accelerator(GTK_WIDGET(self), "accel_f5", priv->accel_group, GDK_KEY_F5,
				0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(GTK_WIDGET(self), "accel_f6", priv->accel_group, GDK_KEY_F6,
				0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(GTK_WIDGET(self), "accel_f7", priv->accel_group, GDK_KEY_F7,
				0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(GTK_WIDGET(self), "accel_f8", priv->accel_group, GDK_KEY_F8,
				0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(GTK_WIDGET(self), "accel_esc", priv->accel_group, GDK_KEY_Escape,
				0, GTK_ACCEL_VISIBLE);

	priv->output_window = hev_output_window_new();

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(self), vbox);

	toolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, TRUE, 0);

	button = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_tool_item_set_tooltip_text(button, "Open file ...");
	g_signal_connect(G_OBJECT(button), "clicked",
				G_CALLBACK(hev_main_window_button_open_file_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), button, -1);

	priv->button_go_back = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_tool_item_set_tooltip_text(priv->button_go_back, "Go back one page");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_go_back), FALSE);
	g_signal_connect(G_OBJECT(priv->button_go_back), "clicked",
				G_CALLBACK(hev_main_window_button_go_back_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_go_back, -1);

	priv->button_go_forward = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_tool_item_set_tooltip_text(priv->button_go_forward, "Go forward one page");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_go_forward), FALSE);
	g_signal_connect(G_OBJECT(priv->button_go_forward), "clicked",
				G_CALLBACK(hev_main_window_button_go_forward_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_go_forward, -1);

	priv->button_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
	gtk_tool_item_set_tooltip_text(priv->button_reload, "Reload current page (F5)");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_reload), FALSE);
	g_signal_connect(G_OBJECT(priv->button_reload), "clicked",
				G_CALLBACK(hev_main_window_button_reload_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_reload, -1);

	priv->button_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP);
	gtk_tool_item_set_tooltip_text(priv->button_stop, "Stop current page");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_stop), FALSE);
	g_signal_connect(G_OBJECT(priv->button_stop), "clicked",
				G_CALLBACK(hev_main_window_button_stop_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_stop, -1);

	priv->button_home = gtk_tool_button_new_from_stock(GTK_STOCK_HOME);
	gtk_tool_item_set_tooltip_text(priv->button_home, "Load home page");
	g_signal_connect(G_OBJECT(priv->button_home), "clicked",
				G_CALLBACK(hev_main_window_button_home_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_home, -1);

	priv->button_execute = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE);
	gtk_tool_item_set_tooltip_text(priv->button_execute, "Execute (F7)");
	gtk_widget_set_sensitive(GTK_WIDGET(priv->button_execute), FALSE);
	g_signal_connect(G_OBJECT(priv->button_execute), "clicked",
				G_CALLBACK(hev_main_window_button_execute_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), priv->button_execute, -1);

	button = gtk_tool_button_new_from_stock(GTK_STOCK_EDIT);
	gtk_tool_item_set_tooltip_text(button, "Output window (F8)");
	g_signal_connect(G_OBJECT(button), "clicked",
				G_CALLBACK(hev_main_window_button_show_output_win_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), button, -1);

	button = gtk_tool_button_new_from_stock(GTK_STOCK_ABOUT);
	gtk_tool_item_set_tooltip_text(button, "About");
	g_signal_connect(G_OBJECT(button), "clicked",
				G_CALLBACK(hev_main_window_button_show_about_dlg_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), button, -1);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	label = gtk_label_new("Location:");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	priv->entry = gtk_entry_new();
	g_signal_connect(G_OBJECT(priv->entry), "activate",
				G_CALLBACK(hev_main_window_entry_real_activate), self);
	gtk_box_pack_start(GTK_BOX(hbox), priv->entry, TRUE, TRUE, 0);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

	priv->web_view = webkit_web_view_new();
	g_signal_connect(G_OBJECT(priv->web_view), "title_changed",
				G_CALLBACK(hev_main_window_web_view_real_title_changed), self);
	g_signal_connect(G_OBJECT(priv->web_view), "load_committed",
				G_CALLBACK(hev_main_window_web_view_real_load_committed), self);
	g_signal_connect(G_OBJECT(priv->web_view), "load_started",
				G_CALLBACK(hev_main_window_web_view_real_load_started), self);
	g_signal_connect(G_OBJECT(priv->web_view), "load_finished",
				G_CALLBACK(hev_main_window_web_view_real_load_finished), self);
	g_signal_connect(G_OBJECT(priv->web_view), "load_progress_changed",
				G_CALLBACK(hev_main_window_web_view_real_load_progress_changed), self);
	g_signal_connect(G_OBJECT(priv->web_view), "create_web_view",
				G_CALLBACK(hev_main_window_web_view_real_create_web_view), self);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
				priv->web_view);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

	priv->label = gtk_label_new("Welcome!");
	gtk_misc_set_alignment(GTK_MISC(priv->label), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox), priv->label, TRUE, TRUE, 0);

	priv->progress_bar = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(hbox), priv->progress_bar, FALSE, TRUE, 0);

	gtk_widget_show_all(GTK_WIDGET(self));
	gtk_widget_hide(priv->progress_bar);
	gtk_window_set_focus(GTK_WINDOW(self), priv->entry);

	home_url = hev_config_get_home_url(HEV_CONFIG(priv->config));
	gtk_entry_set_text(GTK_ENTRY(priv->entry), home_url);
	webkit_web_view_open(WEBKIT_WEB_VIEW(priv->web_view), home_url);
	g_free(home_url);
}

GObject * hev_main_window_new(void)
{
	return g_object_new(HEV_TYPE_MAIN_WINDOW, NULL);
}

