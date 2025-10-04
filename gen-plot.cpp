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

// Numbers from m to n inclusive
//   = sum 1 to n - (sum 1 to m-1)
int sum_m_to_n( int m, int n ) {
  if ( m > n ) {
    return 0;
  }
  return n*(n+1)/2 - (m-1)*(m)/2;
}

int dimensions_to_index( int height, int y, int width, int x ) {
  // height 4: 4 possible y (3 on top row)
  // height 5: 3 possible y
  // height 6: 2 possible y
  // height 7: 1 possible y
  // width 4: 22 possible x
  // width 5: 21 possible x etc.

  // lower bound given by 4 -> 5, 5 -> 4, 6 -> 3
  int y_component = sum_m_to_n( 9 - height, 4 ) + y;

  // lower bound given by 4 -> 23, 5 -> 22, 6 -> 19
  int x_component = sum_m_to_n( 27 - width, 22 ) + x;

  return x_component * 10 + y_component;
}

int index_to_height( int i ) {
  int y_component = i % 10;
  if ( y_component < 4 ) {
    return 4;
  } else if ( y_component < 7 ) {
    return 5;
  } else if ( y_component < 9 ) {
    return 6;
  } else {
    return 7;
  }
}

int index_to_y( int i ) {
  int y_component = i % 10;
  int height = index_to_height( i );
  int y_base = sum_m_to_n( 9 - height, 4 );
  return y_component - y_base;
}

int index_to_width( int i ) {
  int x_component = i / 10;
  for ( int w = 25; w > 4; w-- ) {
    if ( x_component >= sum_m_to_n(27-w, 22) ) {
      return w;
    }
  }
  return 4;
}

int index_to_x( int i ) {
  int x_component = i / 10;
  int width = index_to_width( i );
  int x_base = sum_m_to_n( 27-width, 22 );
  return x_component - x_base;
}

const int NUM_VARIATIONS = 2530;
const bool check_indexing = false;

int
main( int argc, char *argv[] ) {
  std::array<std::array<int,NUM_VARIATIONS>,MAXROOMS> count;
  for ( int y = 0; y < MAXROOMS; y++ ) {
    count[y].fill( 0 );
  }

  Coord bsze;
  bsze.x = NUMCOLS / 3;
  bsze.y = NUMLINES / 3;
  
  for (long seed = 0; seed <= 0xffffffff; seed++ ) {
  // for (long seed = 0; seed <= 0x01ffff; seed++ ) {
    if ( seed % 0x02000000 == 0 ) {
      std::cout << std::hex << seed << std::dec << std::endl;
    }
    auto rooms = gen_rooms( seed, 1 );

    // post-select
    if ( rooms[0].gone || rooms[0].max.x != 4 || rooms[0].max.y != 4 ||
	 rooms[0].pos.x != 1 || rooms[0].pos.y != 1) {
      continue;
    }
    
    if ( rooms[1].gone || rooms[1].max.x != 4 || rooms[1].max.y != 4 ||
	 rooms[1].pos.x != bsze.x + 1 || rooms[1].pos.y != 1) {
      continue;
    }
    
    //if ( rooms[2].gone || rooms[2].max.x != 4 || rooms[2].max.y != 4 ||
    //  	 rooms[2].pos.x != 2 * bsze.x + 1 || rooms[2].pos.y != 1) {
    //   continue;
    // }
    
    for ( int i = 0; i < MAXROOMS; i++ ) {
      Coord top;
      top.x = (i % 3) * bsze.x + 1;
      top.y = (i / 3) * bsze.y;

      if (rooms[i].gone) {
	continue;
      }
      
      int y = rooms[i].pos.y - top.y;
      int x = rooms[i].pos.x - top.x;

      int idx = dimensions_to_index( rooms[i].max.y, y, rooms[i].max.x, x );
      count[i][idx] += 1;
      
      if ( check_indexing ) {
	std::cout << "Room " << i << " height=" << rooms[i].max.y
		  << " y=" << y << " width=" << rooms[i].max.x
		  << " x=" << x << " index=" << idx << "\n";
	
	int hp = index_to_height( idx );
	int wp = index_to_width( idx );
	int yp = index_to_y( idx );
	int xp = index_to_x( idx );
	if ( hp != rooms[i].max.y || wp != rooms[i].max.x ||
	     yp != y || xp != x ) {
	  *(char *)0 = 0;
	}
      }
    }
  }

  for ( int i = 0; i < MAXROOMS; i++ ) {
    for ( int v = 0; v < NUM_VARIATIONS; v++ ) {
      int h = index_to_height( v );
      int w = index_to_width( v );
      int y = index_to_y( v );
      int x = index_to_x( v );
      std::cout << i << "," << v << ","
		<< h << "," << y << ","
		<< w << "," << x << ","
		<< count[i][v]
		<< "\n";	
    }
  }
  

  return 0;
}

