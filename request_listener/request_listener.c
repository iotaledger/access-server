#include <pthread.h>

#include "auth.h"
#include "auth_logger.h"

#include "request_listener.h"
#include "auth_request_listener.h"

int main() {
  logger_helper_init(LOGGER_INFO);
  logger_init_auth(LOGGER_INFO);

  // test server
  static bool serve = true;
  pthread_t server;
  int ret = pthread_create(&server, NULL, &auth_request_listener_thread, &serve);
  ret = pthread_detach(server);

  // wait
  sleep(1000);

  // kill server
  serve = false;

  return 0;
}