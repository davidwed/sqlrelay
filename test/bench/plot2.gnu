#!/usr/bin/gnuplot

set term pngcairo font "arial,9"
set output 'temp.png'

set datafile separator ','

set title 'Direct '.db.' vs '.db.' via SQL Relay' font ",11"

# grid line style
set linestyle 1 lc rgbcolor '#a9a9a9' lw 1

set grid noxtics nomxtics ytics nomytics ls 1
set border ls 1
set xtics textcolor rgbcolor 'black'
set ytics textcolor rgbcolor 'black'

set key outside right top font ",10"

set xlabel 'Queries-per-connection' font ",10"
set ylabel 'Queries-per-second' font ",10"

# graph line styles
set linestyle 2 lc 'red' lw 2
set linestyle 3 lc 'blue' lw 2

plot \
	'temp.csv' using 1:2 smooth bezier title 'SQL Relay' ls 2, \
	'temp.csv' using 1:3 smooth bezier title db ls 3
