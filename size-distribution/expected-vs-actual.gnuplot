set term png
set output "expected-vs-actual.png"
plot "expected-vs-actual.dat" using 1:5 with points title "actual minus expected, per size"
set output "expected-vs-actual-cumulative.png"
plot "expected-vs-actual.dat" using 1:4 with lines title "actual minus expected, cumulative"
set output "expected-vs-actual-comparison.png"
plot "expected-vs-actual.dat" using 1:4 with lines title "linear congruential", "expected-vs-actual-mersenne.dat" using 1:4 with lines title "Mersenne Twister"

