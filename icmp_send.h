#ifndef ICMP_SEND_H
#define ICMP_SEND_H

#include <netinet/in.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

int send_packet(int sockfd,int ttl, struct sockaddr_in dest, int pid);

#endif