#include "icmp_receive.h"
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

bool is_other_ip_occured(char ips_str[][20], int index) {
	for (int i = index - 1; i >= 0; --i) {
		if (strcmp(ips_str[index], ips_str[i]) != 0)
			return true;
	}
	return false;
}

int receive_packets_from_socket(int pid, int sockfd, int timeout, int ttl, int num_of_packets) {
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(sockfd, &descriptors);
	struct timeval tv = {timeout, 0};
	int packets = 0;

	char sender_ip_str[num_of_packets][20];
	bool is_more_than_one_router = false;
	int j = 0;

	while (tv.tv_sec != 0 || tv.tv_usec != 0) {
		int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
		if (ready < 0)
			return ready;

		for (int i = 0; i < ready; ++i) {
			struct sockaddr_in sender;
			socklen_t sender_len = sizeof(sender);
			u_int8_t buffer[IP_MAXPACKET];

			ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*) &sender, &sender_len);
			if (packet_len < 0) {
				return -1;
			}

			//char sender_ip_str[20]; 
			if (inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str[j], sizeof(sender_ip_str[j])) == NULL) {
				return -1;
			}

			struct ip* ip_header = (struct ip*) buffer;
			u_int8_t* icmp_packet = buffer + 4 * ip_header->ip_hl;

			struct icmp* icmp_header = (struct icmp*) icmp_packet;

			int type = icmp_header->icmp_type;
			if (type != ICMP_TIME_EXCEEDED && type != ICMP_ECHOREPLY)
				continue;
			
			struct icmp* original_icmphdr = icmp_header;
			if (type == ICMP_TIME_EXCEEDED) {
				struct ip* original_iphdr = (void *) icmp_header + 8;
				original_icmphdr = (void *) original_iphdr + 4 * original_iphdr->ip_hl;
			}

			if (original_icmphdr->icmp_hun.ih_idseq.icd_seq != ttl && original_icmphdr->icmp_hun.ih_idseq.icd_id != pid)
				continue;
			
			++packets;

			if (!is_more_than_one_router)
				is_more_than_one_router = is_other_ip_occured(sender_ip_str, j);

			printf("%s\n", sender_ip_str[j]);
			if (type == ICMP_ECHOREPLY)
				return 1;
			++j;
		}
	}

	return 0;
}