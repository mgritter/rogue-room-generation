actual_dist = []
cumulative = 0

with open( "actual-distribution.txt" ) as f:
    for line in f:
        size, count = line.split( ' ' )
        size = int( size )
        count = int( count )
        cumulative += count
        actual_dist.append( (size,cumulative) )
        
expected_dist = []

with open( "expected-distribution.txt" ) as f:
    for line in f:
        if "size" in line:
            continue
        #print( line[0:6] )
        size = int( line[0:6].lstrip() )
        prob = float( line[23:37] )
        expected_count = int( cumulative * prob )
        expected_dist.append( (size, expected_count) )

print( "size", "expected", "actual", "delta", "size_delta" )
i = 0
actual = 0
prev_expected = 0
prev_actual = 0
for (size, expected) in expected_dist:
    if i < len( actual_dist ) and actual_dist[i][0] == size:
        actual = actual_dist[i][1]
        i += 1

    # How big a difference for just this size
    delta = (actual - prev_actual) - (expected - prev_expected)
    
    print( size, expected, actual, actual - expected, delta )
    prev_actual = actual
    prev_expected = expected
        
    #print( f"{total} {float(total/2**32):20}" )
