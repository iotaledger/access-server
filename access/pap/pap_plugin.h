#ifndef _PAPPLUGIN_H_
#define _PAPPLUGIN_H_

#include "pap.h"

#define PAPPLUGIN_MAX_COUNT 5

typedef enum {
  PAPPLUGIN_PUT_CB = 0,
  PAPPLUGIN_GET_CB,
  PAPPLUGIN_HAS_CB,
  PAPPLUGIN_DEL_CB,
  PAPPLUGIN_GET_POL_OBJ_LEN_CB,
  PAPPLUGIN_GET_ALL_CB,
  PAPPLUGIN_CALLBACK_COUNT
} papplugin_e;

typedef struct {
  char policy_id[PAP_POL_ID_MAX_LEN + 1];
  pap_policy_t* policy;
} papplugin_get_args_t;

typedef struct {
  char* policy_id;
  int does_have;
} papplugin_has_args_t;

typedef struct {
  char* policy_id;
  int len;
} papplugin_len_args_t;

#endif
