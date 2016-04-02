#!/usr/bin/gnuplot

set term png
set output 'temp.png'

set datafile separator ','

set title 'Direct '.db.' vs '.db.' via SQL Relay' font ",14"

# grid line style
set linestyle 1 lc rgbcolor '#a9a9a9' lw 1

set grid noxtics nomxtics ytics nomytics ls 1
set border ls 1
set xtics textcolor rgbcolor 'black'
set ytics textcolor rgbcolor 'black'

set key outside right top font ",13"

set xlabel 'Queries-per-connection' font ",13"
set ylabel 'Queries-per-second' font ",13"

# graph line styles
set linestyle 2 lc 'blue' lw 2
set linestyle 3 lc 'red' lw 2

plot \
	'temp.csv' using 1:2 with lines title db ls 2, \
	'temp.csv' using 1:3 with lines title 'SQL Relay' ls 3
