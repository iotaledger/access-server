#ifndef _PEP_PLUGIN_H_
#define _PEP_PLUGIN_H_

#include "pdp.h"
#include "pep.h"

typedef enum {
  PEP_PLUGIN_ACTION_CB = 0,
  PEP_PLUGIN_CALLBACK_COUNT
} pep_plugin_e;

typedef struct {
  pdp_action_t action;
  char obligation[PDP_OBLIGATION_LEN];
} pep_plugin_args_t;

#endif
