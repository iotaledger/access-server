#ifndef CMD_DECISION_H
#define CMD_DECISION_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "cmd_listener.h"

/**
 * @brief calculate decision from command
 * @param cmd command
 * @param cmdlen length of command
 * @param res result
 * @param reslen length of result
 * @return
 */
uint8_t cmd_decision(char *cmd, size_t cmdlen, char *res, size_t *reslen);

#endif  // CMD_DECISION_H
