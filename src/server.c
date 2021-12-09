#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define PORT 2102

int execute(NSIP_Packet *packet) {
    if (checksum(*packet) != packet->checksum)
        return 0;

    FILE *interface = popen("ip route get 1.1.1.1 | grep -Po '(?<=dev\\s)\\w+' | cut -f1 -d ' ' | tr -d \"\n\"", "r");
    char inter[64], ifconfig_inter[128];
    fgets(inter, sizeof(inter), interface);

    printf("Interface: %s", inter);

    FILE *fp;
    switch (packet->query) {
        case MACADDR:
            sprintf(ifconfig_inter, "ifconfig %s | awk '/ether / {print $2}' | cut -d ' ' -f2", inter);
            fp = popen(ifconfig_inter, "r");
            break;
        case RXPACKS:
            sprintf(ifconfig_inter, "ifconfig %s | awk '/TX packets/ {print $3}' | cut -d ':' -f2", inter);
            fp = popen(ifconfig_inter, "r");
            break;
        case TXPACKS:
            sprintf(ifconfig_inter, "ifconfig %s | awk '/RX packets/ {print $3}' | cut -d ':' -f2", inter);
            fp = popen(ifconfig_inter, "r");
            break;
        case RXBYTES:
            sprintf(ifconfig_inter, "ifconfig %s | awk '/TX packets/ {print $5}' | cut -d ':' -f2", inter);
            fp = popen(ifconfig_inter, "r");
            break;
        case TXBYTES:
            sprintf(ifconfig_inter, "ifconfig %s | awk '/RX packets/ {print $5}' | cut -d ':' -f2", inter);
            fp = popen(ifconfig_inter, "r");
            break;
        case TCPPORT:
            fp = popen("netstat -atn4 | wc -l", "r");
            break;
        case UDPPORT:
            fp = popen("netstat -aun4 | wc -l", "r");
            break;
        case TCPLIST:
            fp = popen("netstat -atn4 | grep LISTEN | cut -d ':' -f2 | cut -d ' ' -f1 | tr -s '\n' ',' | sed 's/.$//' | tr -d '\n'", "r");
            break;
        case UDPLIST:
            fp = popen("netstat -aun4 | grep ESTABLISHED | cut -d ':' -f2 | cut -d ' ' -f1 | tr -s '\n' ',' | sed 's/.$//' | tr -d '\n'", "r");
            break;
    }

    fgets((char *)&packet->result, sizeof(packet->result), fp);

    packet->type = NSIP_REP;
    packet->checksum = checksum(*packet);

    return 1;
}

int main() {
    NSIP_Packet received_packet;
    struct sockaddr_in sv_addr, cl_addr;
    int len = sizeof(struct sockaddr_in);

    memset(&sv_addr, 0, sizeof(sv_addr));
    memset(&cl_addr, 0, sizeof(cl_addr));

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(2102);

    if (bind(fd, (const struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
        printf("[!] Bind falhou!\n");
        exit(1);
    }

    printf("Server running...\n");
    for(EVER) {
        recvfrom(fd,
                 &received_packet,
                 sizeof(received_packet),
                 MSG_WAITALL,
                 (struct sockaddr *)&cl_addr,
                 (socklen_t *)&len);

        log_packet(received_packet);

        if (execute(&received_packet)) {
            sleep((double)0.1);

            log_packet(received_packet);

            sendto(fd,
                   &received_packet,
                   sizeof(received_packet),
                   0,
                   (const struct sockaddr *)&cl_addr,
                   sizeof(cl_addr));
        } else {
            printf("[!] Erro CHECKSUM!");
            log_packet(received_packet);
        }
    }

    close(fd);
    return 0;
}
