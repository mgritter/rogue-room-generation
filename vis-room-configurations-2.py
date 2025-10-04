import cairo
import sys

inputFilename="all-rooms.csv"
if len(sys.argv) > 1:
    inputFilename=sys.argv[1]

# 9 rooms in a 3x3 grid
# Up to 50 cells wide and 51 cells tall in each grid
# Each cell needs to have a number and 7x25 picture

cellWidth = 25 * 2 + 2
labelHeight = 10
cellHeight = 7 * 2 + labelHeight + 2
gridMargin = 10
gridWidth = cellWidth * 50 + gridMargin * 2
gridHeight = cellHeight * 51 + gridMargin * 2

surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, gridWidth * 3, gridHeight * 3)
c = cairo.Context(surface)

c.set_source_rgb(1,1,1)
c.paint()

#  LABEL
# xoooxxxxx...
# xoooxxxxx...

def draw_room( room_num, row, col, height, width, y, x, label,
               bgColor=(0.65,0.65,0.65),
               fgColor=(0,0,1.0)
              ):
    upper_x = gridWidth * (room_num % 3) + cellWidth * col + gridMargin
    upper_y = gridHeight * (room_num // 3) + cellHeight * row + gridMargin

    extents = c.text_extents(label)
    c.set_source_rgb(0,0,0)
    c.select_font_face("FreeSans")
    c.set_font_size(10)
    c.move_to(upper_x + (cellWidth - extents.width) // 2,
              upper_y + extents.height)
    c.show_text(label)
        
    c.set_source_rgb(*bgColor)
    c.rectangle(upper_x,upper_y + labelHeight, 25*2, 7*2)
    c.fill()
    
    c.set_source_rgb(*fgColor)
    c.rectangle(upper_x + x*2, upper_y + labelHeight + y*2, width *2, height*2)
    c.fill()

with open( inputFilename, "r" ) as f:
    for line in f:
        toks = line.split( "," )
        if len(toks) != 7:
            continue
        
        room = int(toks[0])
        feature = int(toks[1])
        height = int(toks[2])
        y = int(toks[3])
        width = int(toks[4])
        x = int(toks[5])
        label = toks[6].rstrip()
        if room <= 1 and label != "0":
            draw_room(room,
                      feature // 50,
                      feature % 50,
                      height,
                      width,
                      y,
                      x,
                      label)
        if label == "0" and room > 1:
            draw_room(room,
                      feature // 50,
                      feature % 50,
                      height,
                      width,
                      y,
                      x,
                      "",
                      fgColor=(0.75,0,0),
                      )

surface.write_to_png("example.png")

