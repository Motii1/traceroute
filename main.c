#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <errno.h>

#define MAX_TTL 30
#define TIMEOUT 64

int main(int argc, char **argv) {
    if (argc != 2) {
        puts("Error: wrong number of arguments");
        return EXIT_FAILURE;
    }

    struct sockaddr_in destination;
    if (inet_pton(AF_INET, argv[1], &destination) != 1) {
        fprintf(stderr, "Wrong argument: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
