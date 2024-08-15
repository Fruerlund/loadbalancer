/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "main.h"

#define MAX_WEB_SERVERS 10
#define MAX_STRING_LENGTH 15

struct options_t options;

TAILQ_HEAD(webservers, entry) head;


/**
 * @brief 
 * 
 * @param options 
 */
void initOptionsStruct(void) {

    memset(options.interfaceName, 0, sizeof(options.interfaceName));
    memset(&options.ip, 0, sizeof(options.ip));
    memset(&options.interfaceHwAddress, 0, sizeof(options.interfaceHwAddress));
}

void updateOptionsStruct(void) {

    struct ifreq s;
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    strcpy(s.ifr_name, options.interfaceName);
    if( ioctl(fd, SIOCGIFHWADDR, &s) == 0) {
        int i;
        for(int i = 0; i < 6; ++i) {
            options.interfaceHwAddress[i] = s.ifr_addr.sa_data[i];
        }
    }


    /* 
     if (ioctl(fd, SIOCGIFADDR, &s) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in *sin = (struct sockaddr_in *)&s.ifr_addr;
    options.ip = sin->sin_addr;
    */

    inet_aton("192.168.1.200", &options.ip);
    if (ioctl(fd, SIOCGIFINDEX, &s) == -1) {
    perror("SIOCGIFINDEX");
    exit(1);
    }
    options.interfaceIndex = s.ifr_ifindex;
    printf("Successfully got interface index: %i\n", options.interfaceIndex);


    close(fd);
}


/**
 * @brief 
 * 
 * @param head 
 * @param servers 
 * @param elements 
 * @return int 
 */
int populateServerTail(char (*servers)[2][MAX_STRING_LENGTH], int numEntries) {

    int i = 0;
    for(i = 0; i < numEntries; i++) {
        struct entry *entry = (struct entry *)malloc(sizeof(struct entry));
        if(entry == NULL) {
            printf("Failed to create entry!\n");
            exit(EXIT_FAILURE);
        }
        entry->index = i;
        printf("Test: %s\n", servers[i][0]);

        inet_aton( (char *)servers[i][0], &entry->serverip);
        memcpy(entry->servermac, &servers[i][1][0], 6);

        TAILQ_INSERT_TAIL(&head, entry, entries);
    }
    printf("Generated %d entries\n", i);
    return i;
}


/**
 * @brief 
 * 
 * @param head 
 */
void displayWebServers(void) {

    printf("---- AVAILABLE WEB SERVERS ----\n");
    struct entry *entry;
    TAILQ_FOREACH(entry, &head, entries) {
        printf("(%d)\t%s\t", entry->index, inet_ntoa(entry->serverip));
        for(int i = 0; i < 6; i++) {
            printf("%02X:", entry->servermac[i]);
        }
        printf("\n");
    }

}


/**
 * @brief 
 * 
 * @param options 
 */
int main(int argc, char **argv) {

    TAILQ_INIT(&head);

    char servers[MAX_WEB_SERVERS][2][MAX_STRING_LENGTH] = {
        {"192.168.56.101", {0x3A, 0x4B, 0x03, 0xA7, 0x41, 0xA7}},
        {"192.168.56.101", {0x3A, 0x4B, 0x03, 0xA7, 0x41, 0xA7}},
        {"192.168.56.101", {0x3A, 0x4B, 0x03, 0xA7, 0x41, 0xA7}},
    };

    populateServerTail(servers, 3);

    srand(time(NULL)); // Seed the random number generator with current time


    initOptionsStruct();
    strcpy(options.interfaceName, "virtual0");
    
    updateOptionsStruct();

    options.interfaceHwAddress[0] = 0x3A;
    options.interfaceHwAddress[1] = 0x4B;
    options.interfaceHwAddress[2] = 0x03;
    options.interfaceHwAddress[3] = 0xA7;
    options.interfaceHwAddress[4] = 0x41;
    options.interfaceHwAddress[5] = 0xA7;
    
    listenOnSocket(5555, &options, &head);
    return 0;
}
