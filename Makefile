PREFIX = /usr/local

PROGNAME := kshell
SRCDIR   := src
BUILDDIR := build

CFLAGS  ?= -O3
CFLAGS  += -Wall -Wextra -pedantic -DPROGNAME="\"$(PROGNAME)\""
LDFLAGS ?= -s

SRCS := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/builtins/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: all
all: $(PROGNAME)

.PHONY: clean
clean:
	rm -f $(PROGNAME)
	rm -rf $(BUILDDIR)

.PHONY: install
install: all
	install -Dm0755 $(PROGNAME) $(DESTDIR)$(PREFIX)/bin/$(BIN)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR) $(BUILDDIR)/builtins:
	mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR) $(BUILDDIR)/builtins
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
