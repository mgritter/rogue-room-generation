import matplotlib.pyplot as plt

fig = plt.figure(figsize=(50,50))
subfigs = fig.subfigures(3,3)

subfigs[0,0].suptitle( "room 0" )

ax2 = subfigs[0,0].subplots(nrows=51,ncols=50)

for row in ax2:
    for a in row:
        a.tick_params('both',
                      bottom=False,
                      labelbottom=False,
                      top=False,
                      labeltop=False,
                      left=False,
                      labelleft=False,
                      right=False,
                      labelright=False)
        for loc, spine in a.spines.items():
            spine.set_visible(False)

present = [0,0,255,255]
not_present=[128,128,128,255]

def rectangle(x,y,w,h,color,bg):
    return [ [ color if i >= x and i < x + w and j >= y and j < y + h else bg
        for i in range( 25 ) ]
      for j in range(7) ]
    
ax2[0,0].set_title( "0" )
ax2[0,0].imshow( rectangle(0,0,4,4,present,not_present) )

ax2[0,1].set_title( "959079" )
ax2[0,1].imshow( rectangle(0,1,4,4,present,not_present) )

ax2[0,2].set_title( "963433" )
ax2[0,2].imshow( rectangle(0,2,4,4,present,not_present) )

ax2[0,3].set_title( "967923" )
ax2[0,3].imshow( rectangle(0,3,4,4,present,not_present) )

plt.show()


