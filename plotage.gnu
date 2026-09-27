set terminal pngcairo size 800,500
set output 'multithreadAxes.png'
set title "Average time taken to create axes"
set xlabel "Elements in each axis"
set ylabel "Average Time [t]"
set grid
set logscale x
set logscale y
plot 'data/inParallel.dat' using 1:2 with linespoints title "inParallel", \
     'data/inSeries.dat' using 1:2 with linespoints title "inSeries"