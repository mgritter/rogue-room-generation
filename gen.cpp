#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>
#include "rng.h"
#include "rooms.h"

void
draw_rooms( const std::array<Room,MAXROOMS> &room ) {
  for (int y = 0; y < NUMLINES; y++) {
    for (int x = 0; x < NUMCOLS; x++) {
      bool found_room = false;
      for (int r = 0; r < MAXROOMS; r++) {
	// Drawing the lines uses +max -1 as the bound,
	// max is the length in each dimension, not a maximum offset
	if ( room[r].pos.y == y &&
	     room[r].pos.x == x &&
	     room[r].gone ) {
	  std::cout << 'G';
	  found_room = true;
	  break;
	}
	
	if ( room[r].pos.y <= y &&
	     y <= room[r].pos.y + room[r].max.y - 1 &&
	     room[r].pos.x <= x &&
	     x <= room[r].pos.x + room[r].max.x - 1 ) {
	  if ( room[r].maze ) {
	    std::cout << 'M';
	  } else if ( room[r].dark ) {
	    std::cout << 'D';
	  } else {
	    std::cout << '.';
	  }
	  found_room = true;
	  break;
	}
      }
      if (!found_room) {
	std::cout << ' ';
      }
    }
    std::cout << '\n';
  }
}

void
count_rooms( const std::array<Room,MAXROOMS> &room, std::array<std::array<long,NUMCOLS>,NUMLINES> &count  ) {
  for (int r = 0; r < MAXROOMS; r++) {
    // Drawing the lines uses +max -1 as the bound,
    // max is the length in each dimension, not a maximum offset
    if (room[r].gone) continue;

    for ( int y = room[r].pos.y ; y <= room[r].pos.y + room[r].max.y - 1; y++ ) {
      for ( int x = room[r].pos.x ; x <= room[r].pos.x + room[r].max.x - 1; x++ ) {
	count[y][x]++;
      }
    }
  }
}

const char *color_hist[10] = {
  "\e[0;90m0",  // intense black
  "\e[0;31m1",  // red
  "\e[0;35m2",  // purple
  "\e[0;34m3",  // blue
  "\e[0;33m4",  // yellow
  "\e[0;93m5",  // intense yellow
  "\e[0;32m6",  // green
  "\e[0;92m7",  // intense green
  "\e[0;37m8",  // white
  "\e[0;97m9",  // intense white
};

template<int Y, int X>
void
draw_distribution( const std::array<std::array<long,X>,Y> &count,
		   long numSamples ) {
  std::cout << numSamples << " samples:\n";
  for ( int y = 0; y < NUMLINES; y++ ) {
    for ( int x = 0; x < NUMCOLS; x++ ) {
      int c = (count[y][x] * 10) / (numSamples);
      if ( c > 9 ) {
	c = 9;
      }
      std::cout << color_hist[c];
    }
    std::cout << '\n';
  }
  std::cout << "\e[0m\n";
}

bool show_postselect = true;
bool show_sizedist = false;

int
main( int argc, char *argv[] ) {
  std::array<std::array<long,NUMCOLS>,NUMLINES> count;
  for ( int y = 0; y < NUMLINES; y++ ) {
    count[y].fill( 0 );
  }
  
  // long samples = 1000000;
  long postselected = 0;
  // for (long seed = 0x20000; seed < 0x20000 + samples; seed++ ) {
  for (long seed = 0x0; seed <= 0xffffffffl; seed++ ) {
    auto rooms = gen_rooms<RogueRng>( seed, 1 );

    if (seed % 0x1000000 == 0 ) {
      std::cout << "seed=" << seed << std::endl;
    }
    if (show_postselect) {
      // EXPERIMENT: postselect those with room 0 a specific size    
      if ( !rooms[0].gone &&
	   rooms[0].max.x == 4 && rooms[0].max.y == 4 ) {
	// std::cout << seed << "\n";
	// draw_rooms( rooms );
	count_rooms( rooms, count );
	postselected += 1;
      }
    }

    if (show_sizedist) {
      if ( !rooms[3].gone ) {
	count[rooms[3].max.y][rooms[3].max.x] += 1;
	postselected += 1;
      }
    }
  }

  if (show_postselect) {
    draw_distribution<NUMLINES,NUMCOLS>( count, postselected );
  }

  if (show_sizedist) {
    for ( int y = 4; y <= 7; y++ ) {
      for ( int x = 4; x <= 25; x++ ) {
	std::cout << y << "x" << x << " " << count[y][x] << "\n";
      }
    }
  }
		    
  return 0;
}

