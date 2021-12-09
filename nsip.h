#ifndef NSIP_H
#define NSIP_H

typedef unsigned char u8;
typedef unsigned short u16;

#define NSIP_REQ 1
#define NSIP_REP 0

#define EVER ;;

typedef struct {
    u8 id;
    u8 type;
    u16 checksum;
    u16 query;
    u8 result[48];
} NSIP_Packet;

typedef enum {
    MACADDR = 1, // Lembrar de começar do zero.
    RXPACKS,
    TXPACKS,
    RXBYTES,
    TXBYTES,
    TCPPORT,
    UDPPORT,
    TCPLIST,
    UDPLIST,
} Query;

#endif