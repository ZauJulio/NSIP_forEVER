#ifndef UTILS_H
#define UTILS_H

typedef unsigned char u8;
typedef unsigned short u16;

#include "nsip.h"

#define EVER ;;

u16 checksum(NSIP_Packet packet);
void log_packet(NSIP_Packet packet);
void help();

#endif
