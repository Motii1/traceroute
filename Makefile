CC=gcc
CFLAGS=-std=c99 -Wextra -Wall

traceroute: traceroute.o
	$(CC) $(CFLAGS) -o traceroute $^

clean:
	rm -rf *.o

distclean:
	rm -rf *.o traceroute
