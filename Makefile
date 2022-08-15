## Copyright @ Behzad Davoodnia
DIR = sina_task_output
TARGET = $(DIR)/main

CC = gcc
LD = gcc

CFLAGS = -O2 -flto
LDFLAGS = -O2 -flto
LIBS = -ljansson -pthread -lm

OBJECTS = $(patsubst %.c,$(DIR)/%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

.PHONY: all default clean run

all: default

default: $(DIR) $(TARGET)

$(DIR):
	mkdir $@

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

$(DIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf $(DIR)

run: default
	$(TARGET)
