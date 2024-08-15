/**
 * @file protocol.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

#define IP_TYPE 0x0800
#define ARP_TYPE 0x0806
#define ETH_P_ALL 0x0003 


/**
 * @brief https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf
 * @return typedef struct 
 */
typedef struct __attribute__((__packed__)) HTTP_HEADER {

} HTTP_HEADER;


typedef struct __attribute__((__packed__)) TCP_HEADER {

    uint16_t source_port;
    uint16_t dest_port;
    uint32_t sequence_number;
    uint32_t ack_number;
    uint8_t data_offset_reserved_flags; // 4 bits for data offset, 3 bits reserved, 9 bits for flags
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;

} TCP_HEADER;


#define IP_VERSION_4 0b01000000
#define PROTOCOL_TCP 0b00000110
#define PROTOCOL_UDP 0b00010001

/**
 * @brief 
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((__packed__)) IP_HEADER {

    uint8_t version;
    uint8_t services;
    uint16_t length;
    uint16_t identification;
    uint16_t flags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    struct in_addr source;
    struct in_addr destination;

} IP_HEADER;

void displayIPheader(struct IP_HEADER *ip);



#define ETH_HEADER_LEN 14

typedef struct __attribute((__packed__)) ETH_HEADER {

    uint8_t h_dest[6];
    uint8_t h_source[6];
    uint16_t h_proto; // CHECK TYPE u_short


} ETH_HEADER;

void displayEthernetHeader(struct ETH_HEADER *eth);


typedef struct __attribute__((__packed__)) ARP_PACKET {

    struct ETH_HEADER eth_header;
    uint16_t hardwaretype;
    uint16_t protocoltype;
    uint8_t hardwaresize;
    uint8_t protocolsize;
    uint16_t opcode;
    uint8_t sendermac[6];
    struct in_addr senderip;
    uint8_t destinationmac[6];
    struct in_addr destinationip;

} ARP_PACKET;

#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2

void displayARPHeader(struct ARP_PACKET *arp);

#endif