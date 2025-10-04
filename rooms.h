#include <array>

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


// This copy of the Rogue room generation algorithm has some missing pieces.
// It doesn't implement the mazes so it can't be used other than on level 1.
// It doesn't place gold or monsters -- consider this a variant in which
// those are done as a second pass (like corridors are in Rogue!)

template<class Rng>
std::array<Room,MAXROOMS>
gen_rooms( int32_t seed, int level ) {
  std::array<Room,MAXROOMS> room;

  Rng rng( seed );
  
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

