#ifndef REQUEST_LISTENER_CALCULATE_DECISION_H
#define REQUEST_LISTENER_CALCULATE_DECISION_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "request_listener.h"

uint8_t request_listener_calculate_decision(char *req, size_t reqlen, char *res, size_t *reslen);

#endif  // REQUEST_LISTENER_CALCULATE_DECISION_H
