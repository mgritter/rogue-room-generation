#include <iostream>
#include "rng.h"
#include "rooms.h"

std::array<int,1051> size_count;

int
main( int argc, char *argv[] ) {
  int min_size = 9*25*25;
  int max_size = 0;
  
  size_count.fill( 0 );
  
  for (long seed = 0; seed <= 0xffffffff; seed++ ) {
  // for (long seed = 0; seed <= 0x01ffff; seed++ ) { // for testing
    if ( seed % 0x02000000 == 0 ) {
      std::cout << std::hex << seed << std::dec << std::endl;
    }
    auto rooms = gen_rooms<RogueRng>( seed, 1 );

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

