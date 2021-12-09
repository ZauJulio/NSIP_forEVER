#include <stdio.h>
#include "utils.h"

int checksum(u8 *buf, int len) {
	int i, sum = 0;
	for (i = 0; i < len; i++) {
		if (i != 2 && i != 3) {
			sum += buf[i];
		}
	}

	sum = sum % 65536;

	return sum;
}

void log_packet(NSIP_Packet packet) {
	printf("id:       %hhu\n", packet.id);
	printf("type:     %hhu\n", packet.type);
	printf("checksum: %hu\n", packet.checksum);
	printf("query:    %hu\n", packet.query);
	printf("result:   %s\n", packet.result);
}