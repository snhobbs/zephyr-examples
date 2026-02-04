/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/dsp/print_format.h>
#include <zephyr/kernel.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util_macro.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "console_server.h"

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/fcntl.h>

/* For Zephyr, keep max number of fd's in sync with max poll() capacity */
#ifdef CONFIG_ZVFS_POLL_MAX
#define NUM_FDS CONFIG_ZVFS_POLL_MAX
#else
#define NUM_FDS 5
#endif

#define BIND_PORT 4242

/* Number of simultaneous client connections will be NUM_FDS be minus 2 */
struct pollfd pollfds[NUM_FDS];
int pollnum;

#define fatal(msg, ...)                                                        \
  {                                                                            \
    printf("Error: " msg "\n", ##__VA_ARGS__);                                 \
    exit(1);                                                                   \
  }

static void setblocking(int fd, bool val) {
  int fl, res;

  fl = fcntl(fd, F_GETFL, 0);
  if (fl == -1) {
    fatal("fcntl(F_GETFL): %d", errno);
  }

  if (val) {
    fl &= ~O_NONBLOCK;
  } else {
    fl |= O_NONBLOCK;
  }

  res = fcntl(fd, F_SETFL, fl);
  if (fl == -1) {
    fatal("fcntl(F_SETFL): %d", errno);
  }
}

int pollfds_add(int fd) {
  int i;
  if (pollnum < NUM_FDS) {
    i = pollnum++;
  } else {
    for (i = 0; i < NUM_FDS; i++) {
      if (pollfds[i].fd < 0) {
        goto found;
      }
    }

    return -1;
  }

found:
  pollfds[i].fd = fd;
  pollfds[i].events = POLLIN;

  return 0;
}

void pollfds_del(int fd) {
  for (int i = 0; i < pollnum; i++) {
    if (pollfds[i].fd == fd) {
      pollfds[i].fd = -1;
      break;
    }
  }
}

int run_server(void) {
  int res;
  static int counter;
  int num_servs = 0;
#if !defined(USE_IPV6) || !(CONFIG_SOC_SERIES_CC32XX)
  int serv4;
  struct sockaddr_in bind_addr4 = {
      .sin_family = AF_INET,
      .sin_port = htons(BIND_PORT),
      .sin_addr =
          {
              .s_addr = htonl(INADDR_ANY),
          },
  };
#endif

  // wait_for_network();

#if !defined(USE_IPV6) || !(CONFIG_SOC_SERIES_CC32XX)
  serv4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serv4 < 0) {
    printf("error: socket: %d\n", errno);
    exit(1);
  }

  res = bind(serv4, (struct sockaddr *)&bind_addr4, sizeof(bind_addr4));
  if (res == -1) {
    printf("Cannot bind IPv4, errno: %d\n", errno);
  }
  num_servs++;

  setblocking(serv4, false);
  listen(serv4, 5);
  pollfds_add(serv4);
#endif

  printf("Asynchronous TCP echo server waits for connections on "
         "port %d...\n",
         BIND_PORT);

  while (1) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char addr_str[32];

    res = poll(pollfds, pollnum, -1);
    if (res == -1) {
      printf("poll error: %d\n", errno);
      continue;
    }

    for (int i = 0; i < pollnum; i++) {
      if (!(pollfds[i].revents & POLLIN)) {
        continue;
      }
      int fd = pollfds[i].fd;
      if (i < num_servs) {
        /* If server socket */
        int client =
            accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
        void *addr = &((struct sockaddr_in *)&client_addr)->sin_addr;

        if (client < 0) {
          printf("error: accept: %d\n", errno);
          continue;
        }
        inet_ntop(client_addr.ss_family, addr, addr_str, sizeof(addr_str));
        printf("Connection #%d from %s fd=%d\n", counter++, addr_str, client);
        if (pollfds_add(client) < 0) {
          static char msg[] = "Too many connections\n";

          res = send(client, msg, sizeof(msg) - 1, 0);
          if (res < 0) {
            printf("error: send: %d\n", errno);
          }
          close(client);
        } else {
          setblocking(client, false);
        }
      } else {
        char buf[128];
        int len = recv(fd, buf, sizeof(buf), 0);
        if (len <= 0) {
          if (len < 0) {
            printf("error: recv: %d\n", errno);
          }
        error:
          pollfds_del(fd);
          close(fd);
          printf("Connection fd=%d closed\n", fd);
        } else {
          int out_len;
          const char *p;
          /* We implement semi-async server,
           * where reads are async, but writes
           * *can* be sync (blocking). Note that
           * in majority of cases they expected
           * to not block, but to be robust, we
           * handle all possibilities.
           */
          setblocking(fd, true);

          for (p = buf; len; len -= out_len) {
            out_len = send(fd, p, len, 0);
            if (out_len < 0) {
              printf("error: "
                     "send: %d\n",
                     errno);
              goto error;
            }
            p += out_len;
          }

          setblocking(fd, false);
        }
      }
    }
  }
  return 0;
}
