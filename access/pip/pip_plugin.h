#ifndef _PEP_PLUGIN_H_
#define _PEP_PLUGIN_H_

#include "pdp.h"
#include "pip.h"

typedef enum {
  PIPPLUGIN_ACQUIRE_CB = 0,
  PIPPLUGIN_START_CB,
  PIPPLUGIN_GET_DATASET_CB,
  PIPPLUGIN_SET_DATASET_CB,
  PIPPLUGIN_CALLBACK_COUNT
} pipplugin_e;

typedef struct {
  char uri[PDP_MAX_STR_LEN];
  pip_attribute_object_t attribute;
} pipplugin_args_t;

typedef struct {
  char *string;
  size_t len;
} pipplugin_string_arg_t;

#endif
