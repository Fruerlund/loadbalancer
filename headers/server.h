/**
 * @file server.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>

#include "protocol.h"
#include "../src/main.h"

#define ERROR -1
#define MAX_CONN 100



int createSocket(int domain, int type, int protocol);
void configureSocket(int fd, char *dev);

#endif