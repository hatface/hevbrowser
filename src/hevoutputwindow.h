/* hevoutputwindow.h
 * Heiher <admin@heiher.info>
 */

#ifndef __HEV_OUTPUT_WINDOW_H__
#define __HEV_OUTPUT_WINDOW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HEV_TYPE_OUTPUT_WINDOW	(hev_output_window_get_type())
#define HEV_OUTPUT_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_OUTPUT_WINDOW, HevOutputWindow))
#define HEV_IS_OUTPUT_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_OUTPUT_WINDOW))
#define HEV_OUTPUT_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_OUTPUT_WINDOW, HevOutputWindowClass))
#define HEV_IS_OUTPUT_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_OUTPUT_WINDOW))
#define HEV_OUTPUT_WINDOW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_OUTPUT_WINDOW, HevOutputWindowclass))

typedef struct _HevOutputWindow HevOutputWindow;
typedef struct _HevOutputWindowClass HevOutputWindowClass;

struct _HevOutputWindow
{
	GtkWindow parent_instance;
};

struct _HevOutputWindowClass
{
	GtkWindowClass parent_class;
};

GType hev_output_window_get_type(void);

GtkWidget * hev_output_window_new(void);
void hev_output_window_append_text(HevOutputWindow * self, const gchar * text,
									gint size);

G_END_DECLS

#endif /* __HEV_OUTPUT_WINDOW_H__ */

