#ifndef ICMP_SEND_H
#define ICMP_SEND_H

#include <stdlib.h>

u_int16_t compute_icmp_checksum(const void *buff, int length);

int send_packet(int sockfd,int ttl, char *ip, int pid);

#endif