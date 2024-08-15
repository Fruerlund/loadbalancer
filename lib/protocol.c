/**
 * @file protocol.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../headers/protocol.h"

void displayIPheader(struct IP_HEADER *ip) {

    
    char *proto;
    switch(ip->protocol) {

        case PROTOCOL_TCP:
            proto = "TCP";
            break;
        case PROTOCOL_UDP:
            proto = "udp";
            break;
        default:
            proto = "Undefined";
            break;
    }

    printf("[IP] \t %s -> %s (%s)\n", inet_ntoa(ip->source), inet_ntoa(ip->destination), proto);

}

void displayEthernetHeader(struct ETH_HEADER *eth) {

    printf("[ETH] \t ");
    printf("Source: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X:", eth->h_source[i]);
    }
    printf(" Destination: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X:", eth->h_dest[i]);
    }
    printf(" ");
    char *proto = "UNDEFINED";
    switch(htons(eth->h_proto)) {
        case IP_TYPE:
            proto = "IP";
            break;
        case ARP_TYPE:
            proto = "ARP";
            break;
        default:
            break;
    }
 
    printf("(%s)\n", proto);


}

void displayARPHeader(struct ARP_PACKET *arp) {

    displayEthernetHeader(&arp->eth_header);

    char ipv4Source[16];
    char ipv4Destination[16];

    uint32_t ipv4SenderBytes = ntohl(arp->senderip.s_addr);
    uint32_t ipv4DestinationBytes = ntohl(arp->destinationip.s_addr);

    sprintf(ipv4Source, "%u.%u.%u.%u",
    (ipv4SenderBytes >> 24) & 0xFF,
    (ipv4SenderBytes >> 16) & 0xFF,
    (ipv4SenderBytes >> 8) & 0xFF,
    ipv4SenderBytes & 0xFF);

    sprintf(ipv4Destination, "%u.%u.%u.%u",
    (ipv4DestinationBytes >> 24) & 0xFF,
    (ipv4DestinationBytes >> 16) & 0xFF,
    (ipv4DestinationBytes >> 8) & 0xFF,
    ipv4DestinationBytes & 0xFF);



    if(htons(arp->opcode) == ARP_OP_REPLY) {
        printf("[ARP-REP]");
    }
    else {
        printf("[ARP-REQ]");
    }
    printf(" Source: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X:", arp->sendermac[i]);
    }
    printf("@%s\t", ipv4Source);
    
    printf("Destination: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X:", arp->destinationmac[i]);
    }
    printf("@%s\n", ipv4Destination);
    

}

