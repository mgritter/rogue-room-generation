#include <iostream>
#include <cstdlib>
#include <cmath>
#include "rng.h"
#include "rooms.h"
#include <fstream>
#include <sstream>
#include <iomanip>

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
    auto rooms = gen_rooms<RogueRng>( seed, 1 );

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

