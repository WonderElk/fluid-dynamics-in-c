CC = gcc
CFLAGS = -Wall -Wextra -g $(shell pkg-config --cflags gsl)
LDLIBS = $(shell pkg-config --libs gsl)

threadTest: threadTest.o
	$(CC) $(CFLAGS) threadTest.o -o threadTest $(LDLIBS)

threadTest.o: threadTest.c
	$(CC) $(CFLAGS) -c threadTest.c

clean:
	rm -f threadTest threadTest.o