CC=gcc
CFLAGS= -Wall -pedantic -std=gnu99
.DEFAULT: all
.PHONY: all debug clean

all: station

station: station.c
	$(CC) $(CFLAG) station.c -o station
