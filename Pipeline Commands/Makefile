AN = proj3
SHELL = /bin/bash
CWD = $(shell pwd | sed 's/.*\///g')

.PHONY: all clean clean-tests zip

all: part1 part2

test: test-part1 test-part2

part1:
	$(MAKE) -C part1

part2:
	$(MAKE) -C part2

test-part1:
	$(MAKE) -C part1 test

test-part2:
	$(MAKE) -C part2 test

clean:
	$(MAKE) -C part1 clean
	$(MAKE) -C part2 clean

clean-tests:
	$(MAKE) -C part1 clean-tests
	$(MAKE) -C part2 clean-tests

zip: clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi
