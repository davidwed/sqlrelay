#!/usr/bin/gnuplot

set term pngcairo font "arial,9"
set output 'temp.png'

set datafile separator ','

set title db font ",11"

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

plot \
	'temp.csv' using 1:2 smooth bezier title db ls 2
