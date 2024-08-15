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

#include "../headers/server.h"


/**
 * @brief Create a Socket object
 * 
 * @return int 
 */
int createSocket(int domain, int type, int protocol) {
    int fd = -1;
    fd = socket(domain, type, protocol);
    if(fd == -1) {
        printf("[-]: Failed to create socket\n");
        exit(ERROR);
    }
    printf("[+]: File descriptor: %d\n", fd);
    return fd;
}


/**
 * @brief 
 * 
 * @param fd 
 * @return int 
 */
void configureSocket(int fd, char *dev) {
    int opt = 1;
    if( (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, dev, strlen(dev)))) {
        printf("[-]: setsockopt()\n");
        exit(ERROR);
    }
}


void showConfig(struct options_t *options, TAILQ_HEAD(listhead, entry) *webservers) {

    printf("---- AVAILABLE WEB SERVERS ----\n");
    struct entry *entry;
    TAILQ_FOREACH(entry, webservers, entries) {
        printf("(%d)\t%s\t", entry->index, inet_ntoa(entry->serverip));
        for(int i = 0; i < 6; i++) {
            printf("%02X:", entry->servermac[i]);
        }
        printf("\n");
    }

    printf("---- INTERFACE CONFIGURATION ----\n");
    printf("Interface Name: %s\n", options->interfaceName);
    printf("Interface IP Address: %s\n", inet_ntoa(options->ip));
    printf("Interface Hardware Address: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X:", options->interfaceHwAddress[i]);
    }
    printf("\n");

}


/**
 * @brief Starts listening on a given socket.
 * 
 * @return int 
 */
int listenOnSocket(int port, struct options_t *options, TAILQ_HEAD(listhead, entry) *webservers) {

    int fd = createSocket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    char *dev = "virtual0"; // Make this dynamic.
    configureSocket(fd, dev);

    showConfig(options, webservers);
    
    while(1) {
        readSocket(fd, options, webservers);
    }
    return 0;
}


/**
 * @brief Reads data from a listening socket.
 * 
 * @return int 
 */
void readSocket(int fd, struct options_t *options, TAILQ_HEAD(listhead, entry) *webservers) {

    uint32_t buf_size = 65536;
    uint8_t buffer[65536];
  //  memset(buffer, '\x00', buf_size);

    int numbytes = 0;

    numbytes = recvfrom(fd, buffer, buf_size, 0, NULL, NULL);

    /*
    for (size_t i = 0; i < numbytes; i++) {
        printf("%02X ", buffer[i]);

        // Print an extra space every 8 bytes for better readability
        if ((i + 1) % 8 == 0)
            printf(" ");

        // Print a newline every 16 bytes
        if ((i + 1) % 16 == 0)
            printf("\n");
    }

    printf("\n");
    */
    
    handleSocketRequest(fd, buffer, numbytes, options, webservers);

}



/**
 * @brief 
 * 
 * @param senderip 
 * @return int 
 */
int checkCache(char *senderip) {

}


/**
 * @brief 
 * 
 * @param senderip 
 * @return int 
 */
struct entry * selectWebServer(struct IP_HEADER *ip, TAILQ_HEAD(listhead, entry) *webservers) {

    // Check if IP Origin has used previous servers. - Lookup Cache
    /*
    Decide which web server to forward this message to.  You need a simple "load balancing" algorithm that ensures that repeat requests from the same client (by IP address) are always forwarded to the same target web server.  You do not need to worry if the web server is up or down (assuming it is up), nor do you need to try to balance traffic evenly across all servers. A real load balancer would use a more sophisticated algorithm, of course...
    */

    int random_integer = rand() % 2;
    
    printf("Selected: %d\n", random_integer);
    int index = 0;
    int count = 0;
    struct entry *ent;
    TAILQ_FOREACH(ent, webservers, entries) {
        // Check if index i is reached
        if (count == random_integer) {
            break; // Exit the loop
        }
        count++;
    }
    return ent;

}

/* TRY TO INCREASE SEG NUMBER BY 1 !!! TO FORCE GENERATE A NEW TCP CONNECTION ....... */

int forwardPacket(int fd, char *data, uint32_t len, TAILQ_HEAD(listhead, entry) *webservers, struct options_t *options) {

    struct IP_HEADER  *ip = (struct IP_HEADER *) (data + sizeof(struct ETH_HEADER));
    struct ETH_HEADER *eth = (struct ETH_HEADER*)data;
    struct TCP_HEADER *tcp = (struct ETH_HEADER*)(data + (sizeof(struct ETH_HEADER) + sizeof(struct IP_HEADER)));
    

    struct entry *webServer = selectWebServer(ip, webservers);
    printf("Picked webserver %s\n", inet_ntoa(webServer->serverip)); 
    memcpy(eth->h_dest, webServer->servermac, ETH_ALEN);

    tcp->sequence_number = 5;
        printf("Seg: %u\n", tcp->sequence_number);

    /*
    Change the destination MAC address of this packet to the target web server and re-transmit the packet
    */
    struct sockaddr_ll saddr, daddr;
   
    int sock = socket(PF_PACKET , SOCK_RAW , IPPROTO_RAW) ;            //For sending

    memset(&daddr, 0, sizeof(struct sockaddr_ll));
    
    daddr.sll_family = AF_PACKET;
    daddr.sll_ifindex = if_nametoindex("virtual0");
    daddr.sll_protocol = htons(ETH_P_IP);
    daddr.sll_hatype = ARPHRD_ETHER;
    daddr.sll_pkttype = PACKET_OUTGOING; //PACKET_OTHERHOST;
    daddr.sll_halen = ETH_ALEN;

    if (bind(sock, (struct sockaddr*) &daddr, sizeof(daddr)) < 0) {
      perror("bind failed\n");
      close(sock);
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "virtual0");
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
        perror("bind to eth1");
    }



    memcpy(daddr.sll_addr, eth->h_dest, ETH_ALEN); // Destination MAC address
  
    int sent = sendto(fd, data, len, 0, (struct sockaddr *)&daddr, sizeof(daddr));
    printf("Sent %d bytes\n", sent);

}


/**
 * @brief Handles data read from a listening socket parsing into headers.
 * 
 * @param data 
 * @param dataLen 
 * @return int 
 */
int handleSocketRequest(int fd, char *data, uint32_t len, struct options_t *options, TAILQ_HEAD(listhead, entry) *webservers) {

    struct ETH_HEADER *eth = (struct ETH_HEADER *)data;
    struct IP_HEADER  *ip = (struct IP_HEADER *) (data + sizeof(struct ETH_HEADER));
    struct ARP_PACKET *arp = (struct ARP_PACKET *)data;

    switch(htons(eth->h_proto)) {

        /* If the Ethernet frame is an ARP request */
        case ARP_TYPE:
            displayARPHeader(arp);
            /* Check if the ARP request is sent to the Virtual IP address */
            if( (arp->opcode == ARP_OP_REQUEST) == 0) {
                
                if((arp->destinationip.s_addr == options->ip.s_addr) == 1) {

                struct ARP_PACKET arp_reply;
                memset(&arp_reply, '\x00', sizeof(struct ARP_PACKET));

                // Setup Ethernet
                arp_reply.eth_header.h_proto = htons(ETH_P_ARP);                
                memcpy(arp_reply.eth_header.h_dest, eth->h_source, 6);
                memcpy(arp_reply.eth_header.h_source, options->interfaceHwAddress, 6);
                
                // Setup ARP
                arp_reply.hardwaretype = htons(1);
                arp_reply.protocoltype = htons(ETH_P_IP);
                arp_reply.hardwaresize = 6;
                arp_reply.protocolsize = 4;
                arp_reply.opcode = htons(ARP_OP_REPLY);

                // Set IP And MAC
                memcpy(&arp_reply.sendermac, options->interfaceHwAddress, 6);
                memcpy(&arp_reply.destinationmac,  eth->h_source, 6);

                arp_reply.destinationip.s_addr = arp->senderip.s_addr;
                arp_reply.senderip.s_addr = options->ip.s_addr;

                struct sockaddr_ll socket_address;

                memset(&socket_address, 0, sizeof(struct sockaddr_ll));
 
                socket_address.sll_family = AF_PACKET;
                socket_address.sll_protocol = htons(ETH_P_ARP);
                socket_address.sll_ifindex = options->interfaceIndex;
                
                socket_address.sll_hatype = ARPHRD_ETHER;
                socket_address.sll_pkttype = PACKET_OTHERHOST;
                socket_address.sll_halen = ETH_ALEN;
                memcpy(socket_address.sll_addr, arp_reply.eth_header.h_dest, ETH_ALEN); // Destination MAC address
                displayARPHeader(&arp_reply);

                /*
                If yes, generate a raw Ethernet ARP reply and transmit it back to the requester using the fake MAC address (00:11:11:....) owned by the load balancer. The requester will learn this Virtual IP <-> Virtual MAC address pairing.
                */
                int sent = sendto(fd, &arp_reply, sizeof(struct ARP_PACKET), 0, (struct sockaddr *)&socket_address, sizeof(socket_address));

            }
        }
            break;
       
        case IP_TYPE:
            /* If the Ethernet frame is an IP packet (destined to Virtual IP address) + TCP packet (destined to Virtual port number) */
            displayIPheader(ip);
            if((ip->destination.s_addr == options->ip.s_addr) == 1) {
               // forwardPacket(fd, data, len, webservers, options);
            }
            break;
        default:
            break;
    }
}
