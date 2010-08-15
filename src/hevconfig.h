/* hevconfig.h
 * Heiher <admin@heiher.info>
 */

#ifndef __HEV_CONFIG_H__
#define __HEV_CONFIG_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_CONFIG	(hev_config_get_type())
#define HEV_CONFIG(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_CONFIG, HevConfig))
#define HEV_IS_CONFIG(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_CONFIG))
#define HEV_CONFIG_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_CONFIG, HevConfigClass))
#define HEV_IS_CONFIG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_CONFIG))
#define HEV_CONFIG_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_CONFIG, HevConfigclass))

typedef struct _HevConfig HevConfig;
typedef struct _HevConfigClass HevConfigClass;

struct _HevConfig
{
	GObject parent_instance;
};

struct _HevConfigClass
{
	GObjectClass parent_class;
};

GType hev_config_get_type(void);

GObject * hev_config_new(void);

gchar * hev_config_get_def_protocol(HevConfig * self);
gchar * hev_config_get_home_url(HevConfig * self);
gchar * hev_config_get_inner_js_path(HevConfig * self);
gchar * hev_config_get_js_path(HevConfig * self);

G_END_DECLS

#endif /* __HEV_CONFIG_H__ */

