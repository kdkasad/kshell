# BUILD should be either 'release' or 'debug'.
# It is used to set some CFLAGS and LDFLAGS which are used for optimization or
# debugging.
BUILD = debug

PREFIX = /usr/local

CFLAGS = -Wall -Wpedantic $(CFLAGS_$(BUILD))
LDFLAGS = $(LDFLAGS_$(BUILD))
CFLAGS_release = -Os
CFLAGS_debug = -O0 -g
LDFLAGS_release = -s

.PHONY: all
all: kshell

kshell: kshell.c

.PHONY: clean
clean:
	rm -f kshell $(wildcard *.o)

.PHONY: install
install: all
	install -Dm0755 kshell $(DESTDIR)$(PREFIX)/bin/kshell

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/kshell
