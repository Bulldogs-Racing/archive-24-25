CC = gcc
CFLAGS = -Wall -g -O0

all: exec start_button receiver

exec: can_send.o exec.o
	$(CC) $(CFLAGS) -o $@ $^

can_send.o: can_send.c can_send.h
	$(CC) $(CFLAGS) -c $<

exec.o: exec.c can_send.h
	$(CC) $(CFLAGS) -c $<

start_button: start_button.cpp
	g++ start_button.cpp -Wall -o button -llgpio

receiver: can_receive.o pipe_handler.o
	$(CC) $(CFLAGS) -o $@ $^

can_receive.o: can_receive.c pipe_handler.h
	$(CC) $(CFLAGS) -c $<

pipe_handler.o: pipe_handler.c pipe_handler.h
	$(CC) $(CFLAGS) -c $<

