build: archiver.c
	gcc -Wall -D__USE_XOPEN -D_GNU_SOURCE -Wextra archiver.c -o archiver
clean:
	rm -f archiver
