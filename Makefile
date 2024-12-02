GCC = g++
CFLAGS = -O2 -std=c++14
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -march=native
FILES = AGCSketch.out 

all: $(FILES)

frequency.out: frequency.cpp
	$(GCC) $(CFLAGS) frequency.cpp-o frequency.out
heavy_change.out: heavy_change.cpp
    $(GCC) $(CFLAGS) heavy_change.cpp -o heavy_change.out
heavy_hitter.out: heavy_hitter.cpp
    $(GCC) $(CFLAGS) heavy_hitter.cpp -o heavy_hitter.out
throughput.out: throughput.cpp
    $(GCC) $(CFLAGS) throughput.cpp -o throughput.out
clean:
	rm  -rf  *.o  *.out


