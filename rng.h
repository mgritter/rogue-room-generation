#include <cstdint>
#include <random>

// Three random number generators to plug in -- two from actual Rogue implementations,
// and a modern Mersenne Twister.

// Rogue 4.22 version
// https://github.com/Davidslv/rogue/blob/cf9bd26d564a72fac4cf56b55c96c2435270d29a/main.c#L186
class RogueRng {
public:
  RogueRng( int32_t seed ) : seed(seed) {}

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
  PcRogueRng( int32_t seed ) : seed(seed) {}
  
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

