#include "auth.h"

#include "cmd_listener_logger.h"
#include "cmd_decision.h"

uint8_t cmd_decision(char *cmd, size_t cmdlen, char *res, size_t *reslen) {

  char grant_msg[] = "{\"response\":\"access granted\"}";
  char deny_msg[] = "{\"response\":\"access denied\"}";
  char invalid_msg[] = "{\"response\":\"invalid request\"}";

  int8_t request_code;

  if (request_code == 0) {
    char decision[MSGLEN];

    log_info(cmd_listener_logger_id, "[%s:%d] valid cmd, forwarding to PEP.\n", __func__, __LINE__);
    pep_request_access(cmd, (void *)decision);

    if (memcmp(decision, "grant", strlen("grant"))) {
      memcpy(res, grant_msg, sizeof(grant_msg));
    } else {
      memcpy(res, deny_msg, sizeof(deny_msg));
    }

  } else {
    log_info(cmd_listener_logger_id, "[%s:%d] invalid cmd.\n", __func__, __LINE__);
    memcpy(res, invalid_msg, sizeof(invalid_msg));
    return request_code;
  }

  return CMD_LISTENER_OK;
}

/*





}

 */