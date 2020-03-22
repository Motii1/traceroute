#include "icmp_send.h"
#include "icmp_receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_TTL 30
#define PACKETS_TO_SEND 3
#define TIMEOUT 1

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: wrong number of arguments\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in destination;
    if (inet_pton(AF_INET, argv[1], &destination) != 1) {
        fprintf(stderr, "You need to pass a valid IPv4 address!\n");
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("traceroute to: %s\n", argv[1]);

    int pid = getpid();
    int line_number = 1;
    for (int ttl = 1; ttl <= MAX_TTL; ++ttl) {
        struct timeval start_t;
        gettimeofday(&start_t, NULL);

        for (int i = 0; i < PACKETS_TO_SEND; ++i) {
            if (send_packet(sockfd, ttl, argv[1], pid) < 0) {
                fprintf(stderr, "error when sending packets: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }

        int status = receive_packets_from_socket(pid, sockfd, TIMEOUT, ttl, PACKETS_TO_SEND, &start_t, &line_number);
        if (status < 0) {
            fprintf(stderr, "error when receiving packets: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if (status == 1) {
            break;
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
