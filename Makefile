# make				> Build executable with debug flags.
# make run			> Build executable with debug flags, then run it.
# make release		> Build executable with release flags.
# make release run	> Build executable with release flags, then run it.
# make clean		> Remove the executable.

# compiler and flags
CC := gcc
CFLAGS := -march=native -Wall -Wextra -Wshadow -Wundef
CFLAGS_DEBUG := -g3
CFLAGS_RELEASE := -O3
CFLAGS_RUNTIME := $(CFLAGS_DEBUG) $(CFLAGS)

SRCS := spag.c crand.c
EXECUTABLE := spag

ifeq ($(OS),Windows_NT)
EXECUTABLE := $(EXECUTABLE).exe
PATH_SEP := $(subst /,\\,/)
RM := del /q /f
else
PATH_SEP := /
RM := rm -f
endif

.PHONY: all
all: $(EXECUTABLE)
	@echo Building complete.

.PHONY: release
release: _set_release_flags all
.PHONY: _set_release_flags
_set_release_flags:
	$(eval CFLAGS_RUNTIME :=  $(CFLAGS_RELEASE) $(CFLAGS))

.PHONY: run
run: all
	.$(PATH_SEP)$(EXECUTABLE)
	@echo Executing complete.

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE)
	@echo Cleaning complete.

$(EXECUTABLE):
	$(CC) $(CFLAGS_RUNTIME) \
		$(SRCS) \
		-o $(EXECUTABLE)
