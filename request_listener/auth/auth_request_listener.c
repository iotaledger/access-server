#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "sodium.h"

#include "auth.h"
#include "auth_logger.h"

#include "tcpip.h"

// todo: replace this global with a stronghold-based or HSM-based approach
static uint8_t global_ed25519_pk[crypto_sign_PUBLICKEYBYTES];
static uint8_t global_ed25519_sk[crypto_sign_SECRETKEYBYTES];

void *auth_request_listener_thread(bool *serve) {

  // todo: replace this global with a stronghold-based or HSM-based approach
  crypto_sign_keypair(global_ed25519_pk, global_ed25519_sk);

  auth_ctx_t *server = calloc(1, sizeof(auth_ctx_t));

  uint8_t listen_sockfd = tcpip_socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sockfd <= 0){
    log_error(auth_logger_id, "[%s:%d] failed to open listen_sockfd.\n", __func__, __LINE__);
    pthread_exit(NULL);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(9998);

  int retstat = bind(listen_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (retstat != 0) {
    log_error(auth_logger_id, "[%s:%d] bind failed.\n", __func__, __LINE__);
    auth_release(server);
    pthread_exit(NULL);
  }

  retstat = listen(listen_sockfd, 10);
  if (retstat != 0) {
    log_error(auth_logger_id, "[%s:%d] listen failed.\n", __func__, __LINE__);
    auth_release(server);
    pthread_exit(NULL);
  }

  while (serve) {
    /*
    * accept: wait for a connection request
    */
    struct sockaddr_in clientaddr; /* client addr */
    int clientlen = sizeof(clientaddr);
    int accept_sockfd =  tcpip_accept(listen_sockfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (accept_sockfd < 0) {
      log_error(auth_logger_id, "[%s:%d] accept failed.\n", __func__, __LINE__);
      auth_release(server);
      pthread_exit(NULL);
    }

    /*
     * gethostbyaddr: determine who sent the message
     */
    struct hostent *hostp; /* client host info */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                          sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL) {
      log_error(auth_logger_id, "[%s:%d] failed to determine auth client's hostp\n", __func__, __LINE__);
      auth_release(server);
      pthread_exit(NULL);
    }

    char *hostaddrp; /* dotted decimal host addr string */
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL) {
      log_error(auth_logger_id, "[%s:%d] failed to determine auth client's hostaddrp.\n", __func__, __LINE__);
      auth_release(server);
      pthread_exit(NULL);
    }

    log_info(auth_logger_id, "[%s:%d] established connection with %s (%s).\n", __func__, __LINE__, hostp->h_name, hostaddrp);

    if (auth_init_server(server, accept_sockfd) != AUTH_OK) {
      log_error(auth_logger_id, "[%s:%d] failed to init auth server.\n", __func__, __LINE__);
      auth_release(server);
      pthread_exit(NULL);
    }

    // load local_ed25519_sk
    // todo: replace with Stronghold or HSM
    uint8_t local_ed25519_sk[crypto_sign_SECRETKEYBYTES];
    strcpy(local_ed25519_sk, global_ed25519_sk);

    // assumes loaded local_ed25519_sk
    auth_authenticate(server, local_ed25519_sk); // erases local_ed25519_sk

    // load local_ed25519_sk again
    // todo: replace with Stronghold or HSM
    strcpy(local_ed25519_sk, global_ed25519_sk);

    char msg[MSGLEN];

    // assumes loaded local_ed25519_sk
    // auth_receive erases local_ed25519_sk
    if (auth_receive(server, local_ed25519_sk, msg, MSGLEN) != AUTH_OK) {
      log_error(auth_logger_id, "[%s:%d] failed to receive authenticated msg.\n", __func__, __LINE__);
      auth_release(server);
      pthread_exit(NULL);
    }

    log_info(auth_logger_id, "[%s:%d] received authenticated msg: %s\n", __func__, __LINE__, msg);

    // calculate_decision

    // cleanup

    shutdown(accept_sockfd, SHUT_RDWR);
    close(accept_sockfd);

    log_info(auth_logger_id, "[%s:%d] released accept_sockfd.\n", __func__, __LINE__);

  }

  shutdown(listen_sockfd, SHUT_RDWR);
  close(listen_sockfd);
  log_info(auth_logger_id, "[%s:%d] released listen_sockfd.\n", __func__, __LINE__);

  auth_release(server);

  pthread_exit(NULL);
}