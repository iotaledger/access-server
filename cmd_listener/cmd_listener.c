#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "auth.h"
#include "auth_logger.h"

#include "cmd_listener.h"
#include "cmd_listener_logger.h"

#include "auth_cmd_listener.h"

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

int main() {

  signal(SIGINT, signal_handler);
  sigaction(SIGPIPE, &(struct sigaction) {SIG_IGN}, NULL);

  logger_helper_init(LOGGER_INFO);
  logger_init_auth(LOGGER_INFO);
  logger_init_cmd_listener(LOGGER_INFO);

  // start listener/server
  static bool serve = true;
  pthread_t server;

  uint8_t ret = pthread_create(&server, NULL, &auth_cmd_listener_thread, &serve);
  ret = pthread_detach(server);

  // wait
  log_info(cmd_listener_logger_id, "[%s:%d] cmd listener main thread waiting.\n", __func__, __LINE__);
  while (running == 1) usleep(1000);

  // kill server
  serve = false;

  return 0;
}