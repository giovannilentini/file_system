CC = gcc
CFLAGS = -Wall -Iinclude -lm

SRCDIR = src
INCLUDEDIR = include
TESTDIR = test
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.c)
TESTS = $(wildcard $(TESTDIR)/*.c)

OBJS = $(SRCS:$(SRCDIR)/%.c=$(SRCDIR)/%.o)

TEST_EXES = $(patsubst $(TESTDIR)/%.c, $(BINDIR)/%, $(TESTS))

all: $(BINDIR) $(TEST_EXES)

# Create bin directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/%: $(TESTDIR)/%.c $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

# Clean up build artifacts
clean:
	rm -f $(SRCDIR)/*.o $(BINDIR)/*

.PHONY: all clean
