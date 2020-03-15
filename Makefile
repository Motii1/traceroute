CC=gcc
CFLAGS=-std=c99 -Wextra -Wall

traceroute: main.o
	$(CC) $(CFLAGS) -o traceroute $^

clean:
	rm -rf *.o

distclean:
	rm -rf *.o traceroute
