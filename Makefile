# RULES:
# make             -> Build executable with debug flags.
# make run         -> Build executable with debug flags, then run it.
# make release     -> Build executable with release flags.
# make release run -> Build executable with release flags, then run it.
# make clean       -> Remove the executable.
# Use the environment variable ARGS to pass arguments.

# compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Wshadow -Wundef
CFLAGS_DEBUG   := -g3
CFLAGS_RELEASE := -O3
CFLAGS_RUNTIME := $(CFLAGS_DEBUG) $(CFLAGS)

SRCS := spag.c crand.c
EXEC := spag

ifeq ($(OS),Windows_NT)
EXEC := $(EXEC).exe
PATH_SEP := $(subst /,\\,/)
RM := del /q /f
else
PATH_SEP := /
RM := rm -f
endif

# check for apple M1
ARCH := $(shell uname -m)
PLATFORM := $(shell uname -s)
ifeq "$(PLATFORM) $(ARCH)" "Darwin arm64"
	CFLAGS += -mcpu=apple-m1
else
	CFLAGS += -march=native
endif

.PHONY: all release set_release_flags run clean
all: $(EXEC)
	@echo Building complete.

release: set_release_flags all
set_release_flags:
	$(eval CFLAGS_RUNTIME :=  $(CFLAGS_RELEASE) $(CFLAGS))

run: all
	.$(PATH_SEP)$(EXEC) $(ARGS)
	@echo Executing complete.

clean:
	$(RM) $(EXEC)
	@echo Cleaning complete.

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS_RUNTIME) \
		$(SRCS) \
		-o $(EXEC)
