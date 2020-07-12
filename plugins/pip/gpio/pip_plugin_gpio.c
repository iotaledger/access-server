#include "pip_plugin_gpio.h"

static int acquire_cb(plugin_t *plugin, void *user_data) {
  if (plugin == NULL || user_data == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return -1;
  }
  pipplugin_args_t *args = (pipplugin_args_t *)user_data;
  char *uri = args->uri;
  pip_attribute_object_t attribute_object = args->attribute;

  memcpy(attribute_object.type, "int", strlen("int"));
  memcpy(attribute_object.value, gpio_interface_read(0), 1);

  return 0;
}

static int destroy_cb(plugin_t *plugin, void *not_used) {
  free(plugin->callbacks);
  return 0;
}

int pip_plugin_gpio_initializer(plugin_t *plugin, void *user_data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void *) * PIPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks[PIPPLUGIN_ACQUIRE_CB] = acquire_cb;
  plugin->callbacks[PIPPLUGIN_START_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_GET_DATASET_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_SET_DATASET_CB] = NULL;
  plugin->callbacks_num = PIPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
}
