PREFIX := /usr/local

PROGNAME := kshell
SRCDIR   := src
BUILDDIR := build

CFLAGS := -O3 -Wall -Wextra -Wpedantic -Werror -std=c99 -D_POSIX_C_SOURCE=200112L
CPPFLAGS := -DPROGNAME="\"$(PROGNAME)\""

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
	install -Dm0755 $(PROGNAME) $(DESTDIR)$(PREFIX)/bin/$(PROGNAME)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LOADLIBES) $(LDLIBS)

$(BUILDDIR) $(BUILDDIR)/builtins:
	mkdir -p $@

$(OBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR) $(BUILDDIR)/builtins
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
