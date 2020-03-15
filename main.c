#include "icmp_send.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAX_TTL 30
#define TIMEOUT 1000

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

    int pid = getpid();
    const int PACKETS_TO_SEND = 3;
    for (int ttl = 0; ttl < MAX_TTL; ++ttl) {
        for (int i = 0; i < PACKETS_TO_SEND; ++i) {
            if (send_packet(sockfd, ttl, destination, pid) < 0) {
                fprintf(stderr, "sendto error: %s\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
