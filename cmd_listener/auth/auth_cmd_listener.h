#ifndef AUTH_CMD_LISTENER_H
#define AUTH_CMD_LISTENER_H

/**
 * @brief cmd listener thread over authenticated session
 * @param serve flag for pthread_exit
 * @return
 */
void *auth_cmd_listener_thread(bool *serve);

#endif  // AUTH_CMD_LISTENER_H
