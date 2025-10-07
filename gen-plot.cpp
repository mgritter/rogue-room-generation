#include <iostream>
#include <cmath>
#include "rng.h"
#include "rooms.h"

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
    auto rooms = gen_rooms<RogueRng>( seed, 1 );

    // post-select
    // TODO: make this an argument
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

