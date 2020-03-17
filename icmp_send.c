#include "icmp_send.h"
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>

u_int16_t compute_icmp_checksum(const void *buff, int length) {
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int send_packet(int sockfd, int ttl, char *ip, int pid) {
    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = pid;
    header.icmp_hun.ih_idseq.icd_seq = ttl;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum((u_int16_t*) &header, sizeof(header));

    struct sockaddr_in recipent;
    bzero(&recipent, sizeof(recipent));
    recipent.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip, &recipent.sin_addr) < 0)
        return -1;

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0)
        return -1;

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