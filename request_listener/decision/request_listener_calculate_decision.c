#include "auth.h"

#include "request_listener_logger.h"
#include "request_listener_calculate_decision.h"

uint8_t request_listener_calculate_decision(char *req, size_t reqlen, char *res, size_t *reslen) {

  char grant_msg[] = "{\"response\":\"access granted\"}";
  char deny_msg[] = "{\"response\":\"access denied\"}";
  char invalid_msg[] = "{\"response\":\"invalid request\"}";

  int8_t request_code;

  if (request_code == 0) {
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