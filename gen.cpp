#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>

// Rogue 4.22 version
// https://github.com/Davidslv/rogue/blob/cf9bd26d564a72fac4cf56b55c96c2435270d29a/main.c#L186
class RogueRng {
public:
  int32_t seed;

  int rnd(int range) {
    if (range < 1) {
	return 0;
    }
    int val = (this->seed >> 16) & 0xffff;
    this->seed = this->seed*11109+13849;
    // std::cout << this->seed << " " << val << "\n";
    return abs(val) % range;
  }
  
};

// Rogue 1.4 IBM PC port
// https://www.maizure.org/projects/decoded-rogue/MAIN_linenum.txt
class PcRogueRng {
public:
  int32_t seed;

  int ran() {
    this->seed = this->seed * 125;
    this->seed -= (this->seed / 2796203) * 2796203;
    return this->seed;
  }

  int rnd(int range) {
    if (range < 1) {
      return 0;
    }
    return ((this->ran() + this->ran())&0x7fffffffl) % range;
  }
};

const int NUMCOLS = 80;
const int NUMLINES = 24;
const int MAXROOMS = 9;

struct Coord {
  int x;
  int y;
};

bool operator==(const Coord &a, const Coord &b) {
  return a.x == b.x && a.y == b.y;
}

struct Room {
  Room() : gone(false), dark(false), maze(false) {}
  
  Coord pos;
  Coord max;
  bool gone;
  bool dark;
  bool maze;
};

std::array<Room,MAXROOMS>
gen_rooms( int32_t seed, int level ) {
  std::array<Room,MAXROOMS> room;

  RogueRng rng;
  rng.seed = seed;

  Coord bsze;
  bsze.x = NUMCOLS / 3;
  bsze.y = NUMLINES / 3;
  
  // Put in "gone" rooms.  Original code's rnd_room only chooses
  // rooms that aren't gone already
  int left_out = rng.rnd(4);
  for (int i = 0; i < left_out; i++ ) {
    int rm;
    do {
      rm = rng.rnd(MAXROOMS);
    } while (room[rm].gone);
    
    room[rm].gone = true;
  }

  // Flesh out each room
  for (int i = 0; i < MAXROOMS; i++) {
    Coord top;
    top.x = (i % 3) * bsze.x + 1;
    top.y = (i / 3) * bsze.y;

    // Position 'gone' rooms
    if (room[i].gone) {
      do {
	room[i].pos.x = top.x + rng.rnd(bsze.x - 2) + 1;
	room[i].pos.y = top.y + rng.rnd(bsze.y - 2) + 1;
	room[i].max.x = -NUMCOLS;
	room[i].max.y = -NUMLINES;
      } while (!(room[i].pos.y > 0 && room[i].pos.y < NUMLINES-1));
      continue;
    }

    // Decide if a room is a maze or dark
    if (rng.rnd(10) < level - 1 ) {
      room[i].dark = true;
      if (rng.rnd(15) == 0) {
	room[i].maze = true;
      }
    }

    // Size each room
    if (room[i].maze) {
      // Stretch out as much as possible
      room[i].max.x = bsze.x - 1;
      room[i].max.y = bsze.y - 1;
      if ((room[i].pos.x = top.x) == 1) {
	room[i].pos.x = 0;
      }
      if ((room[i].pos.y = top.y) == 0) {
	room[i].pos.y++;
	room[i].max.y--;
      }      
    } else {      
      do {
	room[i].max.x = rng.rnd(bsze.x - 4) + 4;
	room[i].max.y = rng.rnd(bsze.y - 4) + 4;
	room[i].pos.x = top.x + rng.rnd(bsze.x - room[i].max.x);
	room[i].pos.y = top.y + rng.rnd(bsze.y - room[i].max.y);	
      } while (!(room[i].pos.y != 0));
    }

    // FIXME: if there is a maze, a lot more random numbers here we won't attempt to generate.
    // https://github.com/Davidslv/rogue/blob/cf9bd26d564a72fac4cf56b55c96c2435270d29a/rooms.c#L214

    if (false) {
      // Each room may also get gold, a monster, and the monster may have an item.
      // Skip that for now as it brings in too many dependencies.
      
    int gold_amount = 0;
    Coord gold_pos;
    // FIXME: ignores level check and amulet check
    if (rng.rnd(2) == 0) {
      // Add gold
      gold_amount = rng.rnd(50 + 10 * level) + 2;
      gold_pos.x = room[i].pos.x + rng.rnd(room[i].max.x - 2) + 1;
      gold_pos.y = room[i].pos.y + rng.rnd(room[i].max.y - 2) + 1;
      // always empty
    }

    if (rng.rnd(100) < (gold_amount > 0 ? 80 : 25)) {
      // Find position, has to be different than gold_pos
      Coord mob_pos;
      do {
	mob_pos.x = room[i].pos.x + rng.rnd(room[i].max.x - 2) + 1;
	mob_pos.y = room[i].pos.y + rng.rnd(room[i].max.y - 2) + 1;
      } while (mob_pos == gold_pos);
      
      // Roll for monster -- using lvl_mons table, which has no gaps,
      // so we don't need to retry.
      int d = level + (rng.rnd(10) - 6);
      if (d < 0)
	d = rng.rnd(5);
      if (d > 25)
	d = rng.rnd(5) + 21;

      // Monster needs:
      // <level>d8 for hitpoints
      // if X, a disguise
      // a "pack" based on level, which is a random thing
      
    }
    }    
  }
  return room;
}

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
count_rooms( const std::array<Room,MAXROOMS> &room, std::array<std::array<int,NUMCOLS>,NUMLINES> &count  ) {
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
draw_distribution( const std::array<std::array<int,X>,Y> &count,
		   int numSamples ) {
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
  std::array<std::array<int,NUMCOLS>,NUMLINES> count;
  for ( int y = 0; y < NUMLINES; y++ ) {
    count[y].fill( 0 );
  }
  
  int samples = 1000000;
  int postselected = 0;
  for (int seed = 0x20000; seed < 0x20000 + samples; seed++ ) {
    auto rooms = gen_rooms( seed, 1 );

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

