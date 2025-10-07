OPT=-O3 -Wno-aggressive-loop-optimizations

gen: gen.cpp
	g++ -Wall $(OPT) -o gen gen.cpp

gen-bitmaps: gen-bitmaps.cpp
	g++ -Wall $(OPT) -o gen-bitmaps gen-bitmaps.cpp

gen-plot: gen-plot.cpp
	g++ -Wall $(OPT) -o gen-plot gen-plot.cpp

gen-minimum: gen-minimum.cpp
	g++ -Wall $(OPT) -o gen-minimum gen-minimum.cpp

gen-movie: gen-movie.cpp
	g++ -Wall $(OPT) -o gen-movie gen-movie.cpp $(shell pkg-config --cflags --libs cairo)
