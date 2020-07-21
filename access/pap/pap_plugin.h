#ifndef _PAP_PLUGIN_H_
#define _PAP_PLUGIN_H_

#include "pap.h"

#define PAP_PLUGIN_MAX_COUNT 5

typedef enum {
  PAP_PLUGIN_PUT_CB = 0,
  PAP_PLUGIN_GET_CB,
  PAP_PLUGIN_HAS_CB,
  PAP_PLUGIN_DEL_CB,
  PAP_PLUGIN_GET_POL_OBJ_LEN_CB,
  PAP_PLUGIN_GET_ALL_CB,
  PAP_PLUGIN_CALLBACK_COUNT
} pap_plugin_e;

typedef struct {
  char policy_id[PAP_POL_ID_MAX_LEN + 1];
  pap_policy_t* policy;
} pap_plugin_get_args_t;

typedef struct {
  char* policy_id;
  int does_have;
} pap_plugin_has_args_t;

typedef struct {
  char* policy_id;
  int len;
} pap_plugin_len_args_t;

#endif
