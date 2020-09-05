#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "auth.h"
#include "auth_logger.h"

#include "request_listener.h"
#include "request_listener_logger.h"

#include "auth_request_listener.h"

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

int main() {

  signal(SIGINT, signal_handler);
  sigaction(SIGPIPE, &(struct sigaction) {SIG_IGN}, NULL);

  logger_helper_init(LOGGER_INFO);
  logger_init_auth(LOGGER_INFO);
  logger_init_request_listener(LOGGER_INFO);

  // start listener/server
  static bool serve = true;
  pthread_t server;

  int ret = pthread_create(&server, NULL, &auth_request_listener_thread, &serve);
  ret = pthread_detach(server);

  // wait
  log_info(request_listener_logger_id, "[%s:%d] request listener main thread waiting.\n", __func__, __LINE__);
  while (running == 1) usleep(1000);

  // kill server
  serve = false;

  return 0;
}