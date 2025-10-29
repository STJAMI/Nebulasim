# Makefile for NebulaSim
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
SRCDIR = src
OBJ = $(SRCDIR)/main.o $(SRCDIR)/nebula.o
TARGET = NebulaSim

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(SRCDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/nebula.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $(SRCDIR)/main.o

$(SRCDIR)/nebula.o: $(SRCDIR)/nebula.c $(SRCDIR)/nebula.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/nebula.c -o $(SRCDIR)/nebula.o

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
`