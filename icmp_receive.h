#ifndef ICMP_RECEIVE_H
#define ICMP_RECEIVE_H

#include <stdlib.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <sys/time.h>

bool is_other_ip_occured(char ips_str[][20], int index);

void print_avg_time(struct timeval* times, int n);

int receive_packets_from_socket(int pid, int sockfd, int timeout, int ttl,
                                int num_of_packets, struct timeval* start_time, int* line_number);

#endif