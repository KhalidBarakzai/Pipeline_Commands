CFLAGS = -Wall -Werror -g
CC = gcc $(CFLAGS)

all: swish run_terminal_session

swish: swish.c string_vector.o swish_funcs.o swish_funcs_provided.o
	$(CC) -o $@ $^

string_vector.o: string_vector.h string_vector.c
	$(CC) -c string_vector.c

swish_funcs.o: string_vector.o swish_funcs.c
	$(CC) -c swish_funcs.c

run_terminal_session: run_terminal_session.c
	$(CC) -o $@ $^ -lutil

clean:
	rm -f string_vector.o swish_funcs.o swish run_terminal_session

test-setup:
	@chmod u+x testy

test: test-setup swish run_terminal_session
	./testy test_swish.org $(testnum)

clean-tests:
	rm -rf test-results

zip:
	@echo "ERROR: You cannot run 'make zip' from the part2 subdirectory. Change to the main proj3-code directory and run 'make zip' there."
