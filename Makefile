CC=gcc
CFLAGS=-std=gnu99 -Wextra -Wall

traceroute: main.o icmp_send.o icmp_receive.o
	$(CC) $(CFLAGS) -o traceroute $^

clean:
	rm -rf *.o

distclean:
	rm -rf *.o traceroute
