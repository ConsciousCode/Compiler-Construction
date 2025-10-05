CC = g++
LEVEL = -g
CFLAGS = -Wall -Wextra -Werror -fno-exceptions -fno-rtti $(LEVEL)
OUT = out

$(OUT)/test_scanner: src/test_scanner.cpp src/scanner.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OUT)/test_scanner

all: $(OUT)/test_scanner

.PHONY: clean all
