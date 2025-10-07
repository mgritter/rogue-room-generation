#include "rng.h"
#include "rooms.h"
#include <cairo.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

void
draw_rooms( const std::array<Room,MAXROOMS> &rooms, const std::string &filename ) {
  // 80x25 display
  // 8x12 font?
  int width = 8;
  int height = 12;
  cairo_surface_t *surface = cairo_image_surface_create( CAIRO_FORMAT_RGB24, 80 * width, 25 * height );
  cairo_t *c = cairo_create( surface );
  cairo_surface_destroy( surface );

  cairo_set_source_rgb(c, 1.0, 1.0, 1.0);
  cairo_paint( c );

  cairo_select_font_face( c, "FreeMono", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL );
  cairo_set_font_size( c, 12.0 );

  cairo_text_extents_t extents;
  cairo_text_extents( c, "#", &extents );
  // std::cout << "width " << extents.width << " height " << extents.height << std::endl;
  // Offset y values from the upper-left corner down to the baseline.
  int y_offs = extents.height;
  
  cairo_set_source_rgb(c, 0.0, 0.0, 0.0);
  for (int r = 0; r < MAXROOMS; r++) {
    if ( rooms[r].gone ) {
      continue;
    }
    for ( int x = rooms[r].pos.x; x < rooms[r].pos.x + rooms[r].max.x; x++ ) {
      cairo_move_to(c,x * width, rooms[r].pos.y * height + y_offs );
      cairo_show_text( c, "#" );
      cairo_move_to(c,x * width, (rooms[r].pos.y + rooms[r].max.y - 1) * height + y_offs );
      cairo_show_text( c, "#" );		   
    }
    for ( int y = rooms[r].pos.y + 1; y < rooms[r].pos.y + rooms[r].max.y - 1; y++ ) {
      cairo_move_to(c, rooms[r].pos.x * width, y * height + y_offs );
      cairo_show_text( c, "#" );
      cairo_move_to(c, (rooms[r].pos.x + rooms[r].max.x - 1) * width, y * height + y_offs );
      cairo_show_text( c, "#" );		   
    }
    for ( int y = rooms[r].pos.y + 1; y < rooms[r].pos.y + rooms[r].max.y - 1; y++ ) {
      for ( int x = rooms[r].pos.x + 1; x < rooms[r].pos.x + rooms[r].max.x - 1; x++ ) {
	cairo_move_to(c, x * width, y * height + y_offs );
	cairo_show_text( c, "." );
      }
    }
  }
  
  cairo_surface_write_to_png( surface, filename.c_str() );
  cairo_destroy( c );
  
}

int main( int argc, char *argv[] ) {
  long num_frames = 100;
  long start_seed = 0;

  if ( argc > 1 ) {
    start_seed = std::strtoul( argv[1], 0, 16 );    
  }

  if ( argc > 2 ) {
    num_frames = std::strtoul( argv[2], 0, 10 );    
  }

  for (long seed = start_seed; seed <= start_seed + num_frames; seed++ ) {
    auto rooms = gen_rooms<RogueRng>(seed, 1 );

    std::stringstream file_name;
    file_name << "movie/frame" << std::setw( 6 ) << std::setfill('0') << (seed - start_seed);
    
    draw_rooms( rooms, file_name.str() );
  }
}

