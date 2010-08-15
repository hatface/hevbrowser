/* hevconfig.c
 * Heiher <admin@heiher.info>
 */

#include "hevconfig.h"

#define HEV_CONFIG_CONF_PATH	"./res/hevconfig.ini"
#define HEV_CONFIG_GROUP		"HevBrowser"
#define HEV_CONFIG_DEF_PROTOCOL	"DefProtocol"
#define HEV_CONFIG_DEF_PROTOCOL_DEF	"http"
#define HEV_CONFIG_HOME_URL		"HomeURL"
#define HEV_CONFIG_HOME_URL_DEF	"http://www.google.com/"
#define HEV_CONFIG_INNER_JS_PATH	"InnerJSPath"
#define HEV_CONFIG_INNER_JS_PATH_DEF	"./res/hevinner.js"
#define HEV_CONFIG_JS_PATH		"JSPath"
#define HEV_CONFIG_JS_PATH_DEF	"./res/hevexecute.js"

#define HEV_CONFIG_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_CONFIG, HevConfigPrivate))

typedef struct _HevConfigPrivate HevConfigPrivate;

struct _HevConfigPrivate
{
	GKeyFile * key_file;
};

G_DEFINE_TYPE(HevConfig, hev_config, G_TYPE_OBJECT);

static void hev_config_dispose(GObject * obj)
{
	HevConfig * self = HEV_CONFIG(obj);
	HevConfigPrivate * priv = HEV_CONFIG_GET_PRIVATE(self);

	if(priv->key_file)
	{
		g_key_file_free(priv->key_file);
		priv->key_file = NULL;
	}

	G_OBJECT_CLASS(hev_config_parent_class)->dispose(obj);
}

static void hev_config_finalize(GObject * obj)
{
	HevConfig * self = HEV_CONFIG(obj);
	HevConfigPrivate * priv = HEV_CONFIG_GET_PRIVATE(self);

	G_OBJECT_CLASS(hev_config_parent_class)->finalize(obj);
}

static void hev_config_class_init(HevConfigClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	obj_class->dispose = hev_config_dispose;
	obj_class->finalize = hev_config_finalize;

	g_type_class_add_private(klass, sizeof(HevConfigPrivate));
}

static void hev_config_init(HevConfig * self)
{
	HevConfigPrivate * priv = HEV_CONFIG_GET_PRIVATE(self);

	priv->key_file = g_key_file_new();

	/* load from file START */
	g_key_file_load_from_file(priv->key_file, HEV_CONFIG_CONF_PATH,
				G_KEY_FILE_NONE, NULL);
	/* load from file END */

	/* set default val START */
	if(!g_key_file_has_key(priv->key_file, HEV_CONFIG_GROUP,
					HEV_CONFIG_DEF_PROTOCOL, NULL))
	{
		g_key_file_set_string(priv->key_file, HEV_CONFIG_GROUP,
					HEV_CONFIG_DEF_PROTOCOL, HEV_CONFIG_DEF_PROTOCOL_DEF);
	}
	if(!g_key_file_has_key(priv->key_file, HEV_CONFIG_GROUP, 
					HEV_CONFIG_HOME_URL, NULL))
	{
		g_key_file_set_string(priv->key_file, HEV_CONFIG_GROUP,
					HEV_CONFIG_HOME_URL, HEV_CONFIG_HOME_URL_DEF);
	}
	if(!g_key_file_has_key(priv->key_file, HEV_CONFIG_GROUP, 
					HEV_CONFIG_INNER_JS_PATH, NULL))
	{
		g_key_file_set_string(priv->key_file, HEV_CONFIG_GROUP,
					HEV_CONFIG_INNER_JS_PATH, HEV_CONFIG_INNER_JS_PATH_DEF);
	}
	if(!g_key_file_has_key(priv->key_file, HEV_CONFIG_GROUP, 
					HEV_CONFIG_JS_PATH, NULL))
	{
		g_key_file_set_string(priv->key_file, HEV_CONFIG_GROUP,
					HEV_CONFIG_JS_PATH, HEV_CONFIG_JS_PATH_DEF);
	}
	/* set default val END */
}

GObject * hev_config_new(void)
{
	return g_object_new(HEV_TYPE_CONFIG, NULL);
}

gchar * hev_config_get_def_protocol(HevConfig * self)
{
	HevConfigPrivate * priv = NULL;

	g_return_val_if_fail(HEV_IS_CONFIG(self), NULL);

	priv = HEV_CONFIG_GET_PRIVATE(self);
	return g_key_file_get_string(priv->key_file, HEV_CONFIG_GROUP,
				HEV_CONFIG_DEF_PROTOCOL, NULL);
}

gchar * hev_config_get_home_url(HevConfig * self)
{
	HevConfigPrivate * priv = NULL;

	g_return_val_if_fail(HEV_IS_CONFIG(self), NULL);

	priv = HEV_CONFIG_GET_PRIVATE(self);
	return g_key_file_get_string(priv->key_file, HEV_CONFIG_GROUP,
				HEV_CONFIG_HOME_URL, NULL);
}

gchar * hev_config_get_inner_js_path(HevConfig * self)
{
	HevConfigPrivate * priv = NULL;

	g_return_val_if_fail(HEV_IS_CONFIG(self), NULL);

	priv = HEV_CONFIG_GET_PRIVATE(self);
	return g_key_file_get_string(priv->key_file, HEV_CONFIG_GROUP,
				HEV_CONFIG_INNER_JS_PATH, NULL);
}

gchar * hev_config_get_js_path(HevConfig * self)
{
	HevConfigPrivate * priv = NULL;

	g_return_val_if_fail(HEV_IS_CONFIG(self), NULL);

	priv = HEV_CONFIG_GET_PRIVATE(self);
	return g_key_file_get_string(priv->key_file, HEV_CONFIG_GROUP,
				HEV_CONFIG_JS_PATH, NULL);
}

