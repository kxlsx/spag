# RULES:
# make             -> Build executable with debug flags.
# make run         -> Build executable with debug flags, then run it.
# make release     -> Build executable with release flags.
# make release run -> Build executable with release flags, then run it.
# make clean       -> Remove the executable.

# compiler and flags
CC := gcc
CFLAGS := -march=native -Wall -Wextra -Wshadow -Wundef
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

.PHONY: all release set_release_flags run clean
all: $(EXEC)
	@echo Building complete.

release: set_release_flags all
set_release_flags:
	$(eval CFLAGS_RUNTIME :=  $(CFLAGS_RELEASE) $(CFLAGS))

run: all
	.$(PATH_SEP)$(EXEC)
	@echo Executing complete.

clean:
	$(RM) $(EXEC)
	@echo Cleaning complete.

$(EXEC):
	$(CC) $(CFLAGS_RUNTIME) \
		$(SRCS) \
		-o $(EXEC)
