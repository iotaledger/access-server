#ifndef _PEP_PLUGIN_H_
#define _PEP_PLUGIN_H_

#include "pdp.h"
#include "pip.h"

typedef enum {
  PIP_PLUGIN_ACQUIRE_CB = 0,
  PIP_PLUGIN_START_CB,
  PIP_PLUGIN_GET_DATASET_CB,
  PIP_PLUGIN_SET_DATASET_CB,
  PIP_PLUGIN_CALLBACK_COUNT
} pip_plugin_e;

typedef struct {
  char uri[PDP_MAX_STR_LEN];
  pip_attribute_object_t attribute;
} pip_plugin_args_t;

typedef struct {
  char *string;
  size_t len;
} pip_plugin_string_arg_t;

#endif
