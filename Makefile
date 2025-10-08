.PHONY: all

OPT=-O3 -Wno-aggressive-loop-optimizations
SRCS=rng.h rooms.h

gen: gen.cpp $(SRCS)
	g++ -Wall $(OPT) -o gen gen.cpp

gen-bitmaps: gen-bitmaps.cpp $(SRCS)
	g++ -Wall $(OPT) -o gen-bitmaps gen-bitmaps.cpp

gen-plot: gen-plot.cpp $(SRCS)
	g++ -Wall $(OPT) -o gen-plot gen-plot.cpp

gen-minimum: gen-minimum.cpp $(SRCS)
	g++ -Wall $(OPT) -o gen-minimum gen-minimum.cpp

gen-movie: gen-movie.cpp $(SRCS)
	g++ -Wall $(OPT) -o gen-movie gen-movie.cpp $(shell pkg-config --cflags --libs cairo)

all: gen gen-bitmaps gen-plot gen-minimum gen-movie
