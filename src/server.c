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

// These macros were defined to only be used inside the "execute" function
// and should not be used in other places.
#define run_fmt(command, inter) (sprintf(ifconfig_inter, command, inter), \
                                 fp = popen(ifconfig_inter, "r"), \
                                 fgets((char *)&packet->result, sizeof(packet->result), fp))
#define run(command) (fp = popen(command, "r"), \
                      fgets((char *)&packet->result, sizeof(packet->result), fp))

#define GET_INTERFACE_CMD "ip route get 1.1.1.1 | grep -Po '(?<=dev\\s)\\w+' | cut -f1 -d ' ' | tr -d '\n'"
#define GET_MACADDR_CMD "ifconfig %s | awk '/ether / {print $2}' | cut -d ' ' -f2"
#define GET_RXPACKS_CMD "ifconfig %s | awk '/TX packets/ {print $3}' | cut -d ':' -f2"
#define GET_TXPACKS_CMD "ifconfig %s | awk '/RX packets/ {print $3}' | cut -d ':' -f2"
#define GET_RXBYTES_CMD "ifconfig %s | awk '/TX packets/ {print $5}' | cut -d ':' -f2"
#define GET_TXBYTES_CMD "ifconfig %s | awk '/RX packets/ {print $5}' | cut -d ':' -f2"
#define GET_TCPPORT_CMD "netstat -atn4 | wc -l"
#define GET_UDPPORT_CMD "netstat -aun4 | wc -l"
#define GET_TCPLIST_CMD "netstat -atn4 | grep LISTEN | cut -d ':' -f2 | cut -d ' ' -f1 \
                        | tr -s '\n' ',' | sed 's/.$//' | tr -d '\n'"
#define GET_UDPLIST_CMD "netstat -aun4 | grep ESTABLISHED | cut -d ':' -f2 \
                        | cut -d ' ' -f1 | tr -s '\n' ',' | sed 's/.$//' | tr -d '\n'"

// Execute the query sent by the client and fill up the packet with the result and new checksum
int execute(NSIP_Packet *packet) {
    if (checksum(*packet) != packet->checksum)
        return 0;

    // Get host's network interface
    FILE *interface = popen(GET_INTERFACE_CMD, "r");
    char inter[64], ifconfig_inter[128];
    fgets(inter, sizeof(inter), interface);

    // The "run" and "run_fmt" macro run the shell  command and copy the result to "packet->result"
    FILE *fp; // WARNING: "fp" is used by "popen" inside "run" and "run_fmt" 
              // macros and should not be changed.
    switch (packet->query) {
    case MACADDR: run_fmt(GET_MACADDR_CMD, inter); break;
    case RXPACKS: run_fmt(GET_RXPACKS_CMD, inter); break;
    case TXPACKS: run_fmt(GET_TXPACKS_CMD, inter); break;
    case RXBYTES: run_fmt(GET_RXBYTES_CMD, inter); break;
    case TXBYTES: run_fmt(GET_TXBYTES_CMD, inter); break;
    case TCPPORT: run(GET_TCPPORT_CMD); break;
    case UDPPORT: run(GET_UDPPORT_CMD); break;
    case TCPLIST: run(GET_TCPLIST_CMD); break;
    case UDPLIST: run(GET_UDPLIST_CMD); break;
    default: return 0;
    }

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
        printf("[!] ERROR: bind() failed.\n");
        exit(1);
    }

    printf("[*] Server running...\n");
    for(EVER) {
        recvfrom(fd,
                 &received_packet,
                 sizeof(received_packet),
                 MSG_WAITALL,
                 (struct sockaddr *)&cl_addr,
                 (socklen_t *)&len);

        printf("[+] Received packet:\n");
        log_packet(received_packet);

        if (execute(&received_packet)) {
            sleep((double)0.1);

            sendto(fd,
                   &received_packet,
                   sizeof(received_packet),
                   0,
                   (const struct sockaddr *)&cl_addr,
                   sizeof(cl_addr));
            printf("[+] Sent packet:\n");
            log_packet(received_packet);
        } else {
            printf("[!] ERROR: Wrong checksum.");
            log_packet(received_packet);
        }
    }

    close(fd);
    return 0;
}
