CFLAGS = -Wall -Werror -g
CC = gcc $(CFLAGS)

all: par_word_lengths

par_word_lengths: par_word_lengths.c
	$(CC) -o $@ $^

clean:
	rm -f par_word_lengths

test-setup:
	@chmod u+x testy

test: test-setup par_word_lengths
	./testy test_par_word_lengths.org $(testnum)

clean-tests:
	rm -rf test-results

zip:
	@echo "ERROR: You cannot run 'make zip' from the part1 subdirectory. Change to the main proj3-code directory and run 'make zip' there."
