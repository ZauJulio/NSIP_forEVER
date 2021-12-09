#ifndef UTILS_H
#define UTILS_H
#include "nsip.h"

int checksum(u8 *buf, int len);
void log_packet(NSIP_Packet packet);
#endif