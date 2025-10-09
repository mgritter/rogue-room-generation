from fractions import Fraction

# rp->r_max.x = rnd(22)+4
# rp->r_max_y = rnd(4)+4
# rp->r_pos.x = top.x + rnd(26 -  rp->r_max.x)
# rp->r_pos.y = top.y + rnd(8 - rp.r_max.y )
# until r.pos.y != 0
#
# top.y = 0  for top row
# 
# P(y=4 | r.pos.y != 0 )
#   = P( r.pos.y != 0 | y=4) * P(y=4) / P( r.pos.y != 0 )
#   = P( rnd( 8 - y ) != 0 ) * 1/4 / P( r.pos.y != 0 )
#   = 3/4 * 1/4 * x
# P(y=5 | r.pos.y != 0 )
#   = 2/3 * 1/4 * x
# P(y=6 | r.pos.y != 0 )
#   = 1/2 * 1/4 * x
# P(y=7 | r.pos.y != 0 )
#   = 0/1 * 1/4 * x 
#
# Sum is 23/48 so x = 48/23
#
# P(y=4 | loop exits) = 9/23
# P(y=5 | ... ) = 8/23
# P(y=6 | ... ) = 6/23


# Compute the distribution of room sizes assuming all choices are equally
# probable betwee 4-7 x 4-25, or 4-6 x 4-25
# 
def single_room_dist( max_height = 7):
    if max_height == 7:
        p_h = {
            4: Fraction(1,4),
            5: Fraction(1,4),
            6: Fraction(1,4),
            7: Fraction(1,4),
        }
    elif max_height == 6:
        p_h = {
            4: Fraction(9,23),
            5: Fraction(8,23),
            6: Fraction(6,23),
        }
        
    res = {}
    p_w = Fraction(1,26-4)
    for i in range( 4, max_height+1 ):
        for j in range( 4, 26 ):
            res[i*j] = res.get(i*j,Fraction()) + p_w * p_h[i]
    return res

def multiply_powerseries( d1, d2 ):
    res = {}
    for (k1,v1) in d1.items():
        for (k2,v2) in d2.items():
            res[k1+k2] = res.get(k1+k2,Fraction()) + v1*v2
    return res

def add_powerseries( *ps ):
    res = ps[0].copy()
    for d in ps[1:]:
        for (k2,v2) in d.items():
            res[k2] = res.get(k2,Fraction()) + v2
    return res
    
def multiply_powerseries_scalar( s, d1 ):
    res = {}
    for (k1,v1) in d1.items():
            res[k1] = s * v1
    return res
    
def six_room_dist():
    p_x = single_room_dist()
    p_x2 = multiply_powerseries( p_x, p_x )
    p_x3 = multiply_powerseries( p_x2, p_x )
    p_x6 = multiply_powerseries( p_x3, p_x3 )
    return p_x6

def six_room_dist_2():
    # Top row can only be height 4, 5, or 6
    dist_top = single_room_dist(max_height = 6)
    # Other rows can have height 7
    dist_lower = single_room_dist(max_height = 7)
    
    # Assume we choose three to be absent (probability about 1/4) that
    # gives us
    # (3/9)*(2/8)*(1/7) that they are all in the top row
    p0 = Fraction(3*2*1, 9*8*7)
    # (6/9)*(3/8)*(2/7) + (3/9)*(6/8)*(2/7) + (3/9)*(2/8)*(6/7) that two
    # are in the top row
    p1 = Fraction(6*3*2 + 3*6*2 + 3*2*6, 9*8*7)
    # (6/9)*(5/8)*(3/7) + (6/9)*(3/8)*(5/7) + (3/9)*(6/8)*(5/7) that one
    # is in the top row.
    p2 = Fraction(6*5*3 + 6*3*5 + 3*6*5, 9*8*7)
    # (6/9)*(5/8)*(4/7) that none are in the top row
    p3 = Fraction(6*5*4, 9*8*7)
    # Checked that these sum to 1
    
    top2 = multiply_powerseries( dist_top, dist_top )
    top3 = multiply_powerseries( top2, dist_top )
    lower2 = multiply_powerseries( dist_lower, dist_lower )
    lower3 = multiply_powerseries( lower2, dist_lower )
    lower4 = multiply_powerseries( lower2, lower2 )
    lower5 = multiply_powerseries( lower4, dist_lower )
    lower6 = multiply_powerseries( lower3, lower3 )
    
    zero_on_top = lower6
    one_on_top = multiply_powerseries( lower5, dist_top )
    two_on_top = multiply_powerseries( lower4, top2 )
    three_on_top = multiply_powerseries( lower3, top3 )

    return add_powerseries(
        multiply_powerseries_scalar( p3, three_on_top ),
        multiply_powerseries_scalar( p2, two_on_top ),
        multiply_powerseries_scalar( p1, one_on_top ),
        multiply_powerseries_scalar( p0, zero_on_top ),
    )
    
x = six_room_dist_2()
print( f"{'size':6} {'prob':15} {'cum':12} {'expected':10}" )

# TODO: this incremental sum is numerically bad
cum = 0.0
for size in sorted(x):
    p = x[size]
    cum += float( p )
    expected = p * 2**30
    if expected < 10:
        print( f"{size:6} {float(p):15.3e} {cum:.12f} {float(expected):10.2}" )
    else:
        print( f"{size:6} {float(p):15.3e} {cum:.12f} {int(expected):10}")
        
    
