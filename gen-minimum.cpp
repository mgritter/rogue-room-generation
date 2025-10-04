#include <iostream>
#include <cstdint>
#include <cmath>
#include <array>
#include <random>

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

class MersenneRng {
public:
  std::mt19937 mt;

  MersenneRng( int32_t seed ) : mt( seed ) {}

  int rnd(int range) {
    if (range < 1) {
      return 0;
    }
    return mt() % range;
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

  // RogueRng rng;
  // rng.seed = seed;

  MersenneRng rng( seed );
  
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

std::array<int,1051> size_count;

int
main( int argc, char *argv[] ) {
  int min_size = 9*25*25;
  int max_size = 0;
  
  size_count.fill( 0 );
  
  for (long seed = 0; seed <= 0xffffffff; seed++ ) {
    // for (long seed = 0; seed <= 0x01ffff; seed++ ) {
    if ( seed % 0x02000000 == 0 ) {
      std::cout << std::hex << seed << std::dec << std::endl;
    }
    auto rooms = gen_rooms( seed, 1 );

    int size = 0;
    int num_gone = 0;
    for ( int i = 0; i < MAXROOMS; i++ ) {
      if ( !rooms[i].gone ) {
	size += rooms[i].max.x * rooms[i].max.y;	  
      } else {
	num_gone += 1;
      }
    }
    // Count only those with three gone rooms
    if ( num_gone == 3  ) {
      size_count[size] += 1;
    }
    
    if ( size < min_size ) {
      min_size = size;
      std::cout << "seed=" << std::hex << seed << std::dec << " size="
		<< size << " ";
      for ( int i = 0; i < MAXROOMS; i++ ) {
	if (rooms[i].gone) {
	  std::cout << "gone ";
	} else {
	  std::cout << "(" << rooms[i].max.x << "," << rooms[i].max.y << ") ";
	}
      }
      std::cout << std::endl;
    }
    
    if ( size > max_size ) {
      max_size = size;
      std::cout << "seed=" << std::hex << seed << std::dec << " size="
		<< size << " ";
      for ( int i = 0; i < MAXROOMS; i++ ) {
	if (rooms[i].gone) {
	  std::cout << "gone ";
	} else {
	  std::cout << "(" << rooms[i].max.x << "," << rooms[i].max.y << ") ";
	}
      }
      std::cout << std::endl;
    }
  }

  std::cout << "Distribution:\n";
  for (int i = 96; i <= 1050; i++ ) {
    if ( size_count[i] != 0 ) {
      std::cout << i << " " << size_count[i] << "\n";
    }
  }
  
  return 0;
}

