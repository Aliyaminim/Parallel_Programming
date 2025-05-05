CC = gcc
CFLAGS = -O2 -Wall -pthread

all: pipe_latency thread_latency

pipe_latency: pipe_latency.c
	$(CC) $(CFLAGS) -o pipe_latency pipe_latency.c

thread_latency: thread_latency.c
	$(CC) $(CFLAGS) -o thread_latency thread_latency.c

clean:
	rm -f pipe_latency thread_latency
