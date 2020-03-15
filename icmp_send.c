#include "icmp_send.h"
#include <assert.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int send_packet(int sockfd, int ttl, struct sockaddr_in dest, int pid) {
    struct icmphdr header;
    header.type = ICMP_ECHO;
    header.code = 0;
    header.un.echo.id = pid;
    header.un.echo.sequence = ttl;
    header.checksum = 0;
    header.checksum = compute_icmp_checksum((u_int16_t*) &header, sizeof(header));

    struct sockaddr_in recipent;
    bzero(&recipent, sizeof(recipent));
    recipent.sin_family = AF_INET;
    recipent.sin_addr = dest.sin_addr;

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    int bytes_sent = sendto(
        sockfd,
        &header,
        sizeof(header),
        0,
        (struct sockaddr*) &recipent,
        sizeof(recipent)
    );

    return bytes_sent;
}