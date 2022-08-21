## Copyright @ Behzad Davoodnia
DIR = sina_task_output
DIR_OUT = output
INCLUDE = include
TARGET = $(DIR)/main

CC = gcc
LD = gcc

CFLAGS = -O2 -flto
LDFLAGS = -O2 -flto
LIBS = -ljansson -pthread -lm

OBJECTS = $(patsubst %.c,$(DIR)/%.o,$(wildcard *.c))
HEADERS = $(INCLUDE)/$(wildcard *.h)


.PHONY: all default clean run

all: default

default: $(DIR) $(TARGET)

$(DIR):
	mkdir $@
	mkdir $(DIR_OUT)

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

$(DIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf $(DIR) $(DIR_OUT)

run: default
	$(TARGET)
