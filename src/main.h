#ifndef MAIN_H
#define MAIN_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <sys/queue.h>

#include "../headers/protocol.h"
#include "../headers/server.h"
#include <time.h>

// Linked list to hold instances of available web servers
struct entry {
    uint8_t servermac[6];
    struct in_addr serverip;
    int index;
    int status;
    TAILQ_ENTRY(entry) entries; // This is required for TAILQ macros
};

typedef struct options_t {

    char interfaceName[100];
    struct in_addr ip;
    uint8_t interfaceHwAddress[6];
    int interfaceIndex;
    struct sockaddr_ll socket_address;


} options_t;

#endif