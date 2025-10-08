This project enumerates all possible 32-bit seeds and feeds them through the
Rogue room generation algorithm.

## Programs

`gen`: counts and plots how often each screen position is occupied.  Can also
show a distribution of room sizes.

`gen-plot`: counts the number of times each room size and room position occurs
in each cell.  Each such (size, position) tuple is a "variation".  Produces a
CSV of the format

```
room number, room variation, height, y offset, width, x offset, count
```

`gen-bitmap`: creates bitmaps for each room measurement characteristic,
with a "1" if the corresponding seed productes that characteristic.  Meant
to be uploaded to S3 for lookups; this proved to be unnecessary.

`gen-minimum`: finds seeds that produce maximal and minimal total room areas.
Also outputs a distribution of total room areas for levels with three "gone"
rooms.

`gen-movie`: plots a sequence of seed values as PNGs for assembly into a
movie or animated GIF.

### Code structure

rooms.h contains a copy of the Rogue room generation algorithm, in C++.
It accepts a pluggable random number generator as a template argument.

rng.h contains three PRNG implementations: original Rogue, DOS port of Rogue,
and a Mersenne Twistre.

Each .cpp file includes these two, generates some levels, and performs
measurements and output.
