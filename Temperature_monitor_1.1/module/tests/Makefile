
TESTS = \
test_logic.c

SRC_DIR := ../Src
SRCS = \
unity/unity.c \
$(SRC_DIR)/logic.c

CFLAGS = \
-I unity \
-I ../Inc \
-DUNITY_OUTPUT_COLOR=1 \
-Wall -Wextra -Werror

OBJS = $(SRCS:.c=.o)
OBJS += $(TESTS:.c=.o)
TEST_BIN := test_bin

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(OBJS)
	gcc -o $@ $(OBJS)

$(OBJS): $(SRCS) $(TESTS)

.PHONY: clean
clean:
	-rm -rf $(TEST_BIN) *.o
