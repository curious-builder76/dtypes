
CC=gcc

CFLAGS=-Wall -Wextra -I.. -ggdb -fsanitize=address

LIBS=-L. -ldtypes 

TARGET=libdtypes.a

SOURCES=$(wildcard *.c)

OBJECTS=$(SOURCES:.c=.o)

BENCH_NAMES=array hashmap hashset lookup trie deque bitset ndarray

BENCH_TARGETS= $(addprefix benchmarks/bench_,$(BENCH_NAMES))


.PHONY: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: bench
bench: build $(BENCH_TARGETS)

.PHONY: purge
purge: clean
	rm -f benchmarks/bench_*

.PHONY: clean
clean:
	rm -f *.o

benchmarks/bench_% : benchmarks/%_benchmark.c
	$(CC) $(CFLAGS)  $< $(LIBS) -o $@
	- $@
