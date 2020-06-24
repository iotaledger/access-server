#ifndef _PEP_PLUGIN_H_
#define _PEP_PLUGIN_H_

#include "pdp.h"
#include "pep.h"

typedef enum { PEPPLUGIN_ACTION_CB = 0, PEPPLUGIN_CALLBACK_COUNT } pepplugin_e;

typedef struct {
  pdp_action_t action;
  char obligation[PDP_OBLIGATION_LEN];
} pepplugin_args_t;

#endif
