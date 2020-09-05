#include "auth.h"

#include "request_listener_logger.h"
#include "request_listener_calculate_decision.h"

int8_t request_listener_check_request(const char *req) {
  if (req == NULL) {
    return REQLIST_DECISION_ERROR_JSON_NULL;
  }

  if (jsonhelper_get_value(req, 0, "cmd") == -1) {
    return REQLIST_DECISION_ERROR_CMD_NOT_FND;
  }

  int cmd = jsonhelper_get_value(req, 0, "cmd");

  if (memcmp(req + jsonhelper_get_token_start(cmd), "resolve", strlen("resolve")) == 0) {
    if (jsonhelper_get_value(req, 0, "policy_id") == -1) {
      // error invalid request
      return REQLIST_DECISION_ERROR_POLID_NOT_FND;
    } else {
      return REQLIST_DECISION_VALID_REQUEST;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "get_policy_list", strlen("get_policy_list")) == 0) {
    if (jsonhelper_get_value(req, 0, "user_id") == -1) {
      // error invalid request
      return REQLIST_DECISION_ERROR_USRID_NOT_FND;
    } else {
      return REQLIST_DECISION_GET_POLICY_LIST;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "enable_policy", strlen("enable_policy")) == 0) {
    if (jsonhelper_get_value(req, 0, "policy_id") == -1) {
      // error invalid request
      return REQLIST_DECISION_ERROR_POLID_NOT_FND;
    } else {
      return REQLIST_DECISION_ENABLE_POL;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "set_dataset", strlen("set_dataset")) == 0) {
    if (jsonhelper_get_value(req, 0, "dataset_list") == -1) {
      return REQLIST_DECISION_ERROR_DATASET_LIST_NOT_FND;
    } else {
      return REQLIST_DECISION_SET_DATASET;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "get_dataset", strlen("get_dataset")) == 0) {
    return REQLIST_DECISION_GET_DATASET;
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "get_user", strlen("get_user")) == 0) {
    if (jsonhelper_get_value(req, 0, "username") == -1) {
      return REQLIST_DECISION_ERROR_GETUSR_NOT_FND;
    } else {
      return REQLIST_DECISION_GET_USER;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "get_auth_user_id", strlen("get_auth_user_id")) == 0) {
    if (jsonhelper_get_value(req, 0, "username") == -1) {
      return REQLIST_DECISION_ERROR_GETUSRID_NOT_FND;
    } else {
      return REQLIST_DECISION_GET_USER_ID;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "register_user", strlen("register_user")) == 0) {
    if (jsonhelper_get_value(req, 0, "user") == -1) {
      return REQLIST_DECISION_ERROR_REGUSR_NOT_FND;
    } else {
      return REQLIST_DECISION_SET_USER;
    }
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "get_all_users", strlen("get_all_users")) == 0) {
    return REQLIST_DECISION_GET_ALL_USERS;
  } else if (memcmp(req + jsonhelper_get_token_start(cmd), "clear_all_users", strlen("clear_all_users")) == 0) {
    return REQLIST_DECISION_CLEAR_ALL_USERS;
  } else {
    return REQLIST_DECISION_ERROR_NEQ_RESOLVE;
  }
}

uint8_t request_listener_calculate_decision(char *req, size_t reqlen, char *res, size_t *reslen) {

  char grant_msg[] = "{\"response\":\"access granted\"}";
  char deny_msg[] = "{\"response\":\"access denied\"}";
  char invalid_msg[] = "{\"response\":\"invalid request\"}";

  int8_t request_code = request_listener_check_request(req);

  if (request_code == REQLIST_DECISION_VALID_REQUEST) {
    char decision[MSGLEN];

    log_info(request_listener_logger_id, "[%s:%d] valid request, forwarding to PEP: %s\n > %s\n", __func__, __LINE__, *req);
    pep_request_access(req, (void *)decision);

    if (memcmp(decision, "grant", strlen("grant"))) {
      memcpy(res, grant_msg, sizeof(grant_msg));
    } else {
      memcpy(res, deny_msg, sizeof(deny_msg));
    }

  } else {
    log_info(request_listener_logger_id, "[%s:%d] invalid request.\n", __func__, __LINE__);
    memcpy(res, invalid_msg, sizeof(invalid_msg));
    return request_code;
  }

  return REQLIST_OK;
}

/*





}

 */