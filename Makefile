.PHONY: all clean test

# Detect OS and set appropriate compiler
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CC = clang
else
    CC = gcc
endif

CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

SRCS = token.c lexer.c ast.c parser.c environment.c repl.c main.c
TEST_SRCS = token.c lexer.c ast.c parser.c environment.c test.c

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)

TARGET = logos
TEST_TARGET = test_logos

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)