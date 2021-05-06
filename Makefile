PREFIX = /usr/local

BIN      := kshell
SRCDIR   := src
BUILDDIR := build

CFLAGS  ?= -O2 -g
CFLAGS  += -Wall -Wextra -pedantic
LDFLAGS ?= -s

SRCS := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/builtins/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	rm -f $(BIN)
	rm -rf $(BUILDDIR)

.PHONY: install
install: all
	install -Dm0755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR) $(BUILDDIR)/builtins:
	mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR) $(BUILDDIR)/builtins
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
