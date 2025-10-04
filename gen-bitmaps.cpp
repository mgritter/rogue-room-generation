#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <array>
#include <fstream>
#include <sstream>
#include <iomanip>

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

const int batch = 1 << 25;

class SerializableBitSet {
private:
  std::array<uint,batch / (8 * sizeof(uint))> storage_;
public:
  SerializableBitSet() : storage_() {
  }

  void write( const std::string &filename ) {
    std::ofstream out( filename.c_str() );
    out.write( reinterpret_cast<const char *>( this->storage_.data() ),
	       this->storage_.size() * sizeof(uint) );
    out.close();
  }

  void clear() {
    this->storage_.fill( 0 );
  }

  void set( uint i ) {
    uint word = i / (8 * sizeof(uint));
    uint bit = i % (8 * sizeof(uint));
    this->storage_[word] |= (1 << bit);
  }
  
};

const int NUM_BITSETS = 462;
  
// Height is 4 to 6 for top row or 4 to 7 for other rows
int bitSetForHeight(int room, int height) {
  int offset = height - 4;
  if (room < 3) {
    return 3 * room + offset;
  }
  return 9 + 4 * room + offset;
}

// Y position relative to bsize is 0 to 2 for top row and 0 to 3 for other rows
int bitSetForPosY(int room, int relative_y) {
  if (room < 3) {
    return 33 + 3 * room + relative_y;
  }
  return 33 + 9 + 4 * room + relative_y;
}

// Width is 4 to 25 for all rows
int bitSetForWidth(int room, int width) {
  return 66 + room * 22 + (width - 4);
}

// X position relative to bsize is 0 to 21
int bitSetForPosX(int room, int relative_x) {
  return 66 + 198 + room * 22 + relative_x;
}

SerializableBitSet *bits[NUM_BITSETS];

int
main( int argc, char *argv[] ) {
  int samples = batch;
  int start_seed = 0;

  if ( argc > 1 ) {
    start_seed = std::strtoul( argv[1], 0, 16 );    
  }
  
  Coord bsze;
  bsze.x = NUMCOLS / 3;
  bsze.y = NUMLINES / 3;
  
  std::cout << "Starting seed is 0x" << std::hex << start_seed << std::dec << "\n";
  std::cout << "Batch size is "<< batch << " samples\n";
  std::cout << "Next seed is 0x" << std::hex << (start_seed + batch) << std::dec << "\n";
  std::cout << "Each bitset is " << sizeof(SerializableBitSet) <<  " bytes\n";
  std::cout << "Total in-memory size is " << sizeof(SerializableBitSet) * NUM_BITSETS / (1024 * 1024) <<  " MiB" << std::endl;
  
  for (int bs = 0; bs < NUM_BITSETS; bs++ ) {
    bits[bs] = new SerializableBitSet();
  }
  
  for (int seed = start_seed; seed < start_seed + samples; seed++ ) {
    auto rooms = gen_rooms( seed, 1 );

    uint which_bit = seed - start_seed;
    for ( int i = 0; i < MAXROOMS; i++ ) {
      Coord top;
      top.x = (i % 3) * bsze.x + 1;
      top.y = (i / 3) * bsze.y;

      if (rooms[i].gone) {
	continue;
      }
      
      int bs = bitSetForHeight(i, rooms[i].max.y);
      bits[bs]->set(which_bit);

      if ( i < 3 ) {
	// pos.y can't be zero, but top.y is
	bs = bitSetForPosY(i, rooms[i].pos.y - top.y - 1 );
      } else {
	bs = bitSetForPosY(i, rooms[i].pos.y - top.y);
      }
      bits[bs]->set(which_bit);

      bs = bitSetForWidth(i, rooms[i].max.x);
      bits[bs]->set(which_bit);
      
      bs = bitSetForPosX(i, rooms[i].pos.x - top.x);
      bits[bs]->set(which_bit);
    }
  }

  std::ostringstream buf;
  buf << "out/" << std::hex << std::setw(8) << std::setfill('0') << start_seed;
  std::string seed_prefix = buf.str();
    
  for ( int i = 0; i < MAXROOMS; i++ ) {
    for ( int h = 4; h <= 7; h++ ) {
      if ( i < 3 && h == 7 ) {
	continue;
      }
      std::ostringstream buf;
      buf << seed_prefix
	  << "-room" << i << "-height" << h;
      int bs = bitSetForHeight(i, h);
      bits[bs]->write( buf.str() );      
    }
    for ( int y = 0; y <= 3; y++ ) {
      if ( i < 3 && y == 3 ) {
	continue;
      }
      std::ostringstream buf;
      buf << seed_prefix
	  << "-room" << i << "-y" << y;
      int bs = bitSetForPosY(i, y);
      bits[bs]->write( buf.str() );      
    }
    for ( int w = 4; w <= 25; w++ ) {
      std::ostringstream buf;
      buf << seed_prefix
	  << "-room" << i << "-width" << w;
      int bs = bitSetForWidth(i, w);
      bits[bs]->write( buf.str() );      
    }
    for ( int x = 0; x <= 21; x++ ) {
      std::ostringstream buf;
      buf << seed_prefix
	  << "-room" << i << "-x" << x;
      int bs = bitSetForPosX(i, x);
      bits[bs]->write( buf.str() );      
    }
  }
  
  return 0;
}

