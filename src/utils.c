#include "utils.h"

#include <stdio.h>

u16 checksum(NSIP_Packet pack) {
    u8 *buf = (u8 *)&pack;

    u16 sum = 0;
    for (size_t i = 0; i < sizeof(NSIP_Packet); i++) {
        if (i != 2 && i != 3) sum += buf[i];
    }

    sum = sum % 65536;
		
    return sum;
}

void help() {
    printf("Try 'HELP' for more information.\n\n");
    printf("MACADDR – MAC address of the network card\n");
    printf("RXPACKS – Number of packages received\n");
    printf("TXPACKS – Number of packages sent\n");
    printf("RXBYTES – Number of bytes received\n");
    printf("TXBYTES – Quantity of bytes sent\n");
    printf("TCPPORT – Number of TCP ports listening to connections\n");
    printf("UDPPORT – Number of UDP ports listening to connections\n");
    printf("TCPLIST – List of TCP ports listening to connections\n");
    printf("UDPLIST – List of UDP ports listening to connections\n");
}

void log_packet(NSIP_Packet packet) {
    printf("id:       %hhu\n", packet.id);
    printf("type:     %hhu\n", packet.type);
    printf("checksum: %hu\n", packet.checksum);
    printf("query:    %hu\n", packet.query);
    printf("result:   %s\n\n", packet.result);
}
