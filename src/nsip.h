#ifndef NSIP_H
#define NSIP_H

#define NSIP_REQ 0x0
#define NSIP_REP 0x1

#define MACADDR 0x0
#define RXPACKS 0x1
#define TXPACKS 0x2
#define RXBYTES 0x3
#define TXBYTES 0x4
#define TCPPORT 0x5
#define UDPPORT 0x6
#define TCPLIST 0x7
#define UDPLIST 0x8
#define HELP    0x9
#define ERROR   0xA

typedef struct {
    u8 id;
    u8 type;
    u16 checksum;
    u16 query;
    u8 result[48];
} NSIP_Packet;

#endif
