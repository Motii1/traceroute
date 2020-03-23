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
#include <sys/time.h>

bool is_other_ip_occured(char ips_str[][20], int index) {
	for (int i = index - 1; i >= 0; --i) {
		if (strcmp(ips_str[index], ips_str[i]) != 0)
			return true;
	}
	return false;
}

void print_avg_time(struct timeval* times, int n) {
	double res = 0;
	for (int i = 0; i < n; ++i) {
		res += times[i].tv_usec + times[i].tv_sec * 1000000.0;
	}
	res /= 1000.0;
	res /= n;

	printf("%.2f ms\n", res);
}

int receive_packets_from_socket(int pid, int sockfd, int timeout, int ttl,
								int num_of_packets, struct timeval* start_time, int* line_number) {
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(sockfd, &descriptors);
	struct timeval tv = {timeout, 0}, current_time;
	int packets = 0;

	char sender_ip_str[num_of_packets][20];
	bool is_more_than_one_router = false;
	bool got_echo_reply = false;

	struct timeval times[num_of_packets];
	for (int i = 0; i < num_of_packets; ++i) {
		times[i].tv_sec = 0;
		times[i].tv_usec = 0;
	}


	while ((tv.tv_sec != 0 || tv.tv_usec != 0) && packets < num_of_packets) {
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

			if (inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str[packets], sizeof(sender_ip_str[packets])) == NULL) {
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

			if (!is_more_than_one_router)
				is_more_than_one_router = is_other_ip_occured(sender_ip_str, packets);

			gettimeofday(&current_time, NULL);
			timersub(&current_time, start_time, &times[packets]);
			++packets;

			if (type == ICMP_ECHOREPLY) {
				got_echo_reply = true;
				tv.tv_sec = 0;
				tv.tv_usec = 0;
				break;
			}
		}
	}

	printf("%d  ", *line_number);
	*line_number = *line_number + 1;
	if (packets == 0) {
		printf("*\n");
	} else if (!is_more_than_one_router) {
		printf("%s  ", sender_ip_str[0]);

		if (packets == num_of_packets || got_echo_reply)
			print_avg_time(times, packets);
		else
			printf("???\n");
	} else {
		for (int i = 0; i < packets; ++i) {
			printf("%s  \n", sender_ip_str[i]);
		}
		if (packets == num_of_packets)
			print_avg_time(times, packets);
		else
			printf("???\n");
		print_avg_time(times, packets);
	}
	return got_echo_reply ? 1 : 0;
}