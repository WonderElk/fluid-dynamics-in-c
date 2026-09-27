set terminal pngcairo size 800,500
set output 'tThread.png'
set title "Average time taken to create axes"
set xlabel "Elements in each axis"
set ylabel "Average Time [t]"
set grid
set logscale x
set logscale y
plot 'solveParallel.dat' using 1:2 with linespoints title "inParallel", \
     'solveSeries.dat' using 1:2 with linespoints title "inSeries"