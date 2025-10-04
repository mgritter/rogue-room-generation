#include <cstdint>
#include <iostream>

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

int
main( int argc, char *argv[] ) {
  int64_t seed = 0;
  int max_loops = 0;
  
  do {
    RogueRng rng;
    rng.seed = int32_t(seed);
    int left_out = rng.rnd( 4 );
    if ( left_out == 2 ) {
      int gone_1 = rng.rnd( 9 );
      int num_loops = 0;
      int gone_2;
      do {
	gone_2 = rng.rnd( 9 );
	num_loops += 1;
      } while (gone_1 == gone_2);
      // Count only the number of extra loops
      num_loops -= 1;
      
      if ( num_loops > max_loops ) {
	std::cout << num_loops << " retries with seed " << seed << "\n";
	max_loops = num_loops;
      }
    }    
    seed++;

    if ( false && seed % 10000000 == 0 ) {
      std::cout << "at seed " << seed << "\n";
    }
    // Loop until seed space is exhausted
  } while (seed <= 0xffffffffl);
}
