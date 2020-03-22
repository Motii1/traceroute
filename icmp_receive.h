#ifndef ICMP_RECEIVE_H
#define ICMP_RECEIVE_H

#include <stdlib.h>
#include <netinet/ip.h>
#include <stdbool.h>

bool is_other_ip_occured(char ips_str[][20], int index);

int receive_packets_from_socket(int pid, int sockfd, int timeout, int ttl, int num_of_packets);

#endif