#ifndef NSIP_H
#define NSIP_H

typedef unsigned char u8;
typedef unsigned short u16;

#define NSIP_REQ 0
#define NSIP_REP 1

typedef struct {
    u8 id;
    u8 type;
    u16 checksum;
    u16 query;
    u8 result[48];
} NSIP_Packet;

enum {
    MACADDR,
    RXPACKS,
    TXPACKS,
    RXBYTES,
    TXBYTES,
    TCPPORT,
    UDPPORT,
    TCPLIST,
    UDPLIST,
};

#endif