#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "utils.h"

#define PORT 2102

u16 get_command(const char *command) {
    u16 query = 0;
    if (!strcmp(command, "MACADDR"))      query = MACADDR;
    else if (!strcmp(command, "RXPACKS")) query = RXPACKS;
    else if (!strcmp(command, "TXPACKS")) query = TXPACKS;
    else if (!strcmp(command, "RXBYTES")) query = RXBYTES;
    else if (!strcmp(command, "TXBYTES")) query = TXBYTES;
    else if (!strcmp(command, "TCPPORT")) query = TCPPORT;
    else if (!strcmp(command, "UDPPORT")) query = UDPPORT;
    else if (!strcmp(command, "TCPLIST")) query = TCPLIST;
    else if (!strcmp(command, "UDPLIST")) query = UDPLIST;
    else if (!strcmp(command, "HELP")) {
        help();
        query = -1;
    } else {
        printf("[!] ERROR: Command not found.\n");
        printf("Try 'HELP' for more information.\n");
        query = -1;
    }
    return query;
}


NSIP_Packet assemble_packet(int query) {
    NSIP_Packet new_packet;

    memset(&new_packet, 0, sizeof(new_packet));

    srand(time(NULL));

    new_packet.id = (u8)rand();
    new_packet.type = NSIP_REQ;
    new_packet.query = query;
    new_packet.checksum = checksum(new_packet);

    return new_packet;
}

void test_checksum() {
    NSIP_Packet packet = assemble_packet(2);

    assert(packet.type == NSIP_REQ);
    assert(packet.checksum == checksum(packet));

    packet.query <<= 1;
    assert(packet.checksum != checksum(packet));
}

int main() {
    // Tests
    test_checksum();

    char command[64];
    int len, fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in sv_addr, cl_addr;
    NSIP_Packet received_packet;

    memset(&sv_addr, 0, sizeof(sv_addr));

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(PORT);
    sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    for (EVER) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = 0;

        u16 query = get_command(command);
        if (query == (u16)-1) continue;

        NSIP_Packet packet = assemble_packet(query);

        sendto(fd,
               &packet,
               sizeof(packet),
               MSG_CONFIRM,
               (const struct sockaddr *)&sv_addr,
               sizeof(sv_addr));

        recvfrom(fd,
                 &received_packet,
                 sizeof(received_packet),
                 MSG_WAITALL,
                 (struct sockaddr *)&cl_addr,
                 (socklen_t *)&len);

        if (checksum(received_packet) == received_packet.checksum) {
            printf("%s\n", received_packet.result);
        } else {
            printf("[!] Erro CHECKSUM!");
            log_packet(received_packet);
        }
    }

    close(fd);
    return 0;
}
