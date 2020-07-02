#ifndef _PIP_PLUGIN_WALLET_H_
#define _PIP_PLUGIN_WALLET_H_

#include "pip_plugin.h"
#include "wallet.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PROTOCOL_MAX_STR_LEN 256

int pippluginwallet_initializer(plugin_t *plugin, void *user_data);

#endif
