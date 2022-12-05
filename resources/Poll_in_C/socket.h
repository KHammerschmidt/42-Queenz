#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>//close
#include <string.h>//memset-memcpy

#define SERVER_PORT 12345

#define TRUE    1
#define FALSE   0

#endif