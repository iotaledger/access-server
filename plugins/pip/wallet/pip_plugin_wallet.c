#include "pip_plugin_wallet.h"

#include "pthread.h"

#define PROTOCOL_TRANSACTION_NOT_PAID 0
#define PROTOCOL_TRANSACTION_PAID 1
#define PROTOCOL_TRANSACTION_PAID_VERIFIED 2
#define PROTOCOL_WALLET_ADDR_LEN 81
#define PROTOCOL_MAX_STR_LEN 256

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum { TRANS_NOT_PAYED, TRANS_PAYED, TRANS_PAYED_VERIFIED } transaction_payment_state_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct serv_confirm {
  confirmation_service_t *service;
  char *policy_id;
  int policy_id_len;
  bool transaction_confirmed;
} transaction_serv_confirm_t;

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define TRANS_CONF_SERV_MAX_NUM 64
#define TRANS_INTERVAL_S 30
#define TRANS_TIMEOUT_S 120

/****************************************************************************
 * GLOBAL VARIBLES
 ****************************************************************************/
static transaction_serv_confirm_t service[TRANS_CONF_SERV_MAX_NUM] = {0};
static pthread_mutex_t trans_mutex;
static wallet_ctx_t *dev_wallet;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static void transaction_confirmation(uint32_t time, bool is_confirmed, pthread_t thread_id) {
  int i;

  // Find the service
  for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++) {
    if (service[i].service->thread_id == thread_id) {
      // Found
      break;
    }
  }

  if (i == TRANS_CONF_SERV_MAX_NUM) {
    return;
  }

#ifdef USE_RPI
  if (!rpitransaction_update_payment_status(service[i].policy_id, service[i].policy_id_len, is_confirmed)) {
    printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
    return;
  }
#else
  // Add support for other platforms
#endif

  service[i].transaction_confirmed = TRUE;
}

static int recover_transaction(char *policy_id, int policy_id_len) {
  // Check input parameters
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
    return TRANS_NOT_PAYED;
  }

  pthread_mutex_lock(&trans_mutex);

  // Check transaction status
#ifdef USE_RPI
  if (!rpitransaction_is_stored(policy_id)) {
    pthread_mutex_unlock(&trans_mutex);
    return TRANS_NOT_PAYED;
  } else {
    if (!rpitransaction_is_verified(policy_id, policy_id_len)) {
      pthread_mutex_unlock(&trans_mutex);
      return TRANS_PAYED;
    } else {
      pthread_mutex_unlock(&trans_mutex);
      return TRANS_PAYED_VERIFIED;
    }
  }
#else
  // Add support for other platforms
#endif
}

static int destroy_cb(plugin_t *plugin, void *not_used) {
  free(plugin->callbacks);
  return 0;
}

static int acquire_cb(plugin_t *plugin, void *user_data) {
  if (plugin == NULL || user_data == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return -1;
  }

  pipplugin_args_t *args = (pipplugin_args_t *)user_data;
  char *uri = args->uri;
  pip_attribute_object_t attribute_object = args->attribute;

  char temp[PROTOCOL_MAX_STR_LEN];
  char pol_id[PROTOCOL_MAX_STR_LEN];
  char type[PROTOCOL_MAX_STR_LEN];
  char value[PROTOCOL_MAX_STR_LEN];
  char *ptr = NULL;

  // Parse uri
  if (strlen(uri) > PROTOCOL_MAX_STR_LEN) {
    printf("\nERROR[%s]: URI too long.\n", __FUNCTION__);
    return -1;
  } else {
    memcpy(temp, uri, strlen(uri));
  }

  ptr = strtok(temp, ":");

  if (memcmp(ptr, "iota", strlen("iota")) != 0)  // Only supported authority for now
  {
    printf("\nERROR[%s]: Authority not supported.\n", __FUNCTION__);
    return -1;
  }

  ptr = strtok(NULL, "/");
  memcpy(pol_id, ptr, strlen(ptr) > PROTOCOL_MAX_STR_LEN ? PROTOCOL_MAX_STR_LEN : strlen(ptr));
  ptr = strtok(NULL, "?");
  memcpy(type, ptr, strlen(ptr) > PROTOCOL_MAX_STR_LEN ? PROTOCOL_MAX_STR_LEN : strlen(ptr));
  ptr = strtok(NULL, "?");
  memcpy(value, ptr, strlen(ptr) > PROTOCOL_MAX_STR_LEN ? PROTOCOL_MAX_STR_LEN : strlen(ptr));

  if (memcmp(type, "request.isPayed.type", strlen("request.isPayed.type")) == 0) {
    memcpy(attribute_object.type, "string", strlen("string"));
    if (recover_transaction != NULL) {
      int ret = recover_transaction(pol_id, strlen(pol_id));

      if (ret == PROTOCOL_TRANSACTION_PAID_VERIFIED) {
        memcpy(attribute_object.value, "verified", strlen("verified"));
      } else if (ret == PROTOCOL_TRANSACTION_PAID) {
        memcpy(attribute_object.value, "paid", strlen("paid"));
      } else {
        memcpy(attribute_object.value, "not_paid", strlen("not_paid"));
      }
    }
  } else if (memcmp(type, "request.walletAddress.type", strlen("request.walletAddress.type")) == 0) {
    char addr_buf[PROTOCOL_WALLET_ADDR_LEN] = {0};
    uint64_t index;
    wallet_get_address(dev_wallet, addr_buf, &index);

    memcpy(attribute_object.type, "string", strlen("string"));
    memcpy(attribute_object.value, addr_buf, PROTOCOL_WALLET_ADDR_LEN);
  }

  return 0;
}

int pippluginwallet_initializer(plugin_t *plugin, void *user_data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void *) * PIPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks[PIPPLUGIN_ACQUIRE_CB] = acquire_cb;
  plugin->callbacks[PIPPLUGIN_START_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_GET_DATASET_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_SET_DATASET_CB] = NULL;
  plugin->callbacks_num = PIPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
}
