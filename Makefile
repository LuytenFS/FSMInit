CC = gcc

BUILD ?= DEBUG

ifeq ($(BUILD),DEBUG)
    CFLAGS = -g -O0 -Wall
else ifeq ($(BUILD),RELEASE)
    CFLAGS = -O2 -Wall
endif

SRCDIR = src
OBJDIR = build/$(BUILD)
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all: FSMINIT

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

FSMINIT: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf build/ FSMINIT
