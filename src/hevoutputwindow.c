/* hevoutputwindow.c
 * Heiher <admin@heiher.info>
 */

#include "hevoutputwindow.h"

#define HEV_OUTPUT_WINDOW_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_OUTPUT_WINDOW, HevOutputWindowPrivate))

typedef struct _HevOutputWindowPrivate HevOutputWindowPrivate;

struct _HevOutputWindowPrivate
{
	GtkWidget * text_view;
};

G_DEFINE_TYPE(HevOutputWindow, hev_output_window, GTK_TYPE_WINDOW);

static void hev_output_window_real_destroy(GtkObject * obj)
{
	gtk_widget_hide(GTK_WIDGET(obj));
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
	GtkObjectClass * gtk_obj_class = GTK_OBJECT_CLASS(klass);

	obj_class->dispose = hev_output_window_dispose;
	obj_class->finalize = hev_output_window_finalize;

	gtk_obj_class->destroy = hev_output_window_real_destroy;

	g_type_class_add_private(klass, sizeof(HevOutputWindowPrivate));
}

static void hev_output_window_init(HevOutputWindow * self)
{
	HevOutputWindowPrivate * priv = HEV_OUTPUT_WINDOW_GET_PRIVATE(self);
	GtkWidget * scrolled_window = NULL;

	gtk_window_set_title(GTK_WINDOW(self), "HevBrowser Output");
	gtk_window_set_position(GTK_WINDOW(self), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_widget_set_size_request(GTK_WIDGET(self), 500, 300);
	gtk_container_set_border_width(GTK_CONTAINER(self), 2);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
				GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(self), scrolled_window);

	priv->text_view = gtk_text_view_new();
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
				priv->text_view);

	gtk_widget_show_all(scrolled_window);
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

