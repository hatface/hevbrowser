/* hevoutputwindow.c
 * Heiher <admin@heiher.info>
 */

#include <string.h>

#include "hevoutputwindow.h"

#define HEV_OUTPUT_WINDOW_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_OUTPUT_WINDOW, HevOutputWindowPrivate))

typedef struct _HevOutputWindowPrivate HevOutputWindowPrivate;

struct _HevOutputWindowPrivate
{
	GtkWidget * text_view;
};

G_DEFINE_TYPE(HevOutputWindow, hev_output_window, GTK_TYPE_WINDOW);

static void hev_output_window_button_save_real_clicked(GtkToolButton * button, gpointer data)
{
	HevOutputWindow * window = HEV_OUTPUT_WINDOW(data);
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(window);
	GtkTextBuffer * text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_view));
	GtkWidget * dialog = NULL;
	GtkWidget * hbox = NULL;
	GtkWidget * label = NULL;
	GtkWidget * combo_box_text = NULL;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new("Character Encoding:");
	combo_box_text = gtk_combo_box_text_new();
	/* default encodings START */
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "UTF-8");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "UTF-16");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "GB2312");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "GB18030");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "GBK");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_text), "SHIFT-JIS");
	/* default encodings END */
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box_text), 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), combo_box_text, TRUE, TRUE, 0);
	gtk_widget_show_all(hbox);
	dialog = gtk_file_chooser_dialog_new("Save file",
				GTK_WINDOW(window),
				GTK_FILE_CHOOSER_ACTION_SAVE,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				NULL);
	gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog), hbox);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
	if(GTK_RESPONSE_ACCEPT == gtk_dialog_run(GTK_DIALOG (dialog)))
	{
		gchar * filename = NULL, * text = NULL, * encoding = NULL;
		GtkTextIter start = {0}, end = {0};
		GString * str = NULL;
		gsize i = 0, len = 0;
		gboolean convert_ok = TRUE;

		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);
		encoding = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box_text));
		text = gtk_text_buffer_get_text(text_buffer, &start, &end, TRUE);
		str = g_string_new("");
		len = strlen(text);
		for(i=0; i<len;)
		{
			gsize sr = 0, sw = 0;
			gchar * p = NULL;

			p = g_convert(text+i, len-i, encoding, "UTF-8", &sr, &sw, NULL);
			if(NULL == p)
			{
				GtkWidget * d = gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Convert encoding from UTF-8 to %s failed!", encoding);
				gtk_dialog_run(GTK_DIALOG(d));
				gtk_widget_destroy(d);
				convert_ok = FALSE;
				break;
			}
			else
			{
				g_string_append_len(str, p, sw);
				i += sr;
			}
		}
		if(convert_ok)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			if(!g_file_set_contents(filename, str->str, str->len, NULL))
			{
				GtkWidget * d = gtk_message_dialog_new(GTK_WINDOW(window),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Save to %s failed!", filename);
				gtk_dialog_run(GTK_DIALOG(d));
				gtk_widget_destroy(d);
			}
			g_free(filename);
		}
		g_free(encoding);
		g_free(text);
		g_string_free(str, TRUE);
	}
	gtk_widget_destroy(hbox);
	gtk_widget_destroy(dialog);
}

static void hev_output_window_button_clear_real_clicked(GtkToolButton * button, gpointer data)
{
	HevOutputWindow * window = HEV_OUTPUT_WINDOW(data);
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(window);
	GtkTextBuffer * text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_view));

	gtk_text_buffer_set_text(text_buffer, "", -1);
}

static void hev_output_window_dispose(GObject * obj)
{
	HevOutputWindow * self = HEV_OUTPUT_WINDOW(obj);
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(self);

	G_OBJECT_CLASS(hev_output_window_parent_class)->dispose(obj);
}

static void hev_output_window_finalize(GObject * obj)
{
	HevOutputWindow * self = HEV_OUTPUT_WINDOW(obj);
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(self);

	G_OBJECT_CLASS(hev_output_window_parent_class)->finalize(obj);
}

static void hev_output_window_class_init(HevOutputWindowClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	obj_class->dispose = hev_output_window_dispose;
	obj_class->finalize = hev_output_window_finalize;

	widget_class->destroy = gtk_widget_hide;

	g_type_class_add_private(klass, sizeof(HevOutputWindowPrivate));
}

static void hev_output_window_init(HevOutputWindow * self)
{
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(self);
	GtkWidget * vbox = NULL;
	GtkWidget * toolbar = NULL;
	GtkToolItem * button = NULL;
	GtkWidget * scrolled_window = NULL;

	gtk_window_set_title(GTK_WINDOW(self), "HevBrowser Output");
	gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_widget_set_size_request(GTK_WIDGET(self), 600, 350);
	gtk_container_set_border_width(GTK_CONTAINER(self), 2);
	gtk_window_set_skip_taskbar_hint(GTK_WINDOW(self), TRUE);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(self), vbox);

	toolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, TRUE, 0);

	button = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	g_signal_connect(G_OBJECT(button), "clicked",
				G_CALLBACK(hev_output_window_button_save_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), button, -1);

	button = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR);
	g_signal_connect(G_OBJECT(button), "clicked",
				G_CALLBACK(hev_output_window_button_clear_real_clicked), self);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), button, -1);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

	priv->text_view = gtk_text_view_new();
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
				priv->text_view);

	gtk_widget_show_all(vbox);
	gtk_window_set_focus(GTK_WINDOW(self), priv->text_view);
}

GtkWidget * hev_output_window_new(void)
{
	return gtk_widget_new(HEV_TYPE_OUTPUT_WINDOW, NULL);
}

void hev_output_window_append_text(HevOutputWindow * self, const gchar * text,
									gint size)
{
	HevOutputWindowPrivate * priv = NULL;
	GtkTextBuffer * text_buffer = NULL;
	GtkTextIter text_iter = {0};

	g_return_if_fail(HEV_IS_OUTPUT_WINDOW(self));

	priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(self);
	text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_view));
	gtk_text_buffer_get_end_iter(text_buffer, &text_iter);
	gtk_text_buffer_insert(text_buffer, &text_iter, text, size);
}

