
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

proj2: proj2.c proj2.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o proj2
	rm -f *.o proj2.out
