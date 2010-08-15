/* hevmainwindow.h
 * Heiher <admin@heiher.info>
 */

#ifndef __HEV_MAIN_WINDOW_H__
#define __HEV_MAIN_WINDOW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HEV_TYPE_MAIN_WINDOW	(hev_main_window_get_type())
#define HEV_MAIN_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_MAIN_WINDOW, HevMainWindow))
#define HEV_IS_MAIN_WINDOW(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_MAIN_WINDOW))
#define HEV_MAIN_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_MAIN_WINDOW, HevMainWindowClass))
#define HEV_IS_MAIN_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_MAIN_WINDOW))
#define HEV_MAIN_WINDOW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_MAIN_WINDOW, HevMainWindowclass))

typedef struct _HevMainWindow HevMainWindow;
typedef struct _HevMainWindowClass HevMainWindowClass;

struct _HevMainWindow
{
	GtkWindow parent_instance;
};

struct _HevMainWindowClass
{
	GtkWindowClass parent_class;

	/* signals */
	void (*accel_f5)(HevMainWindow * self);
	void (*accel_f6)(HevMainWindow * self);
	void (*accel_f7)(HevMainWindow * self);
	void (*accel_f8)(HevMainWindow * self);
	void (*accel_esc)(HevMainWindow * self);
};

GType hev_main_window_get_type(void);

GObject * hev_main_window_new(void);

G_END_DECLS

#endif /* __HEV_MAIN_WINDOW_H__ */

