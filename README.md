# Fluid-dynamics-in-C
Discritization of Navier-Stokes and use of multithreading to simulate flow.




#### threadTest.c
Checks how long it takes to generate two axes of varying dim. up to $n$, in series and in parallel (using multithread) respectively. The execution call (after make) is either:
./threadTest - which generates axes of dim n = 2e7
./threadTest <n> - which generate axes of given dim. n

Note: There will be 50 generated axes of varying dim. up to n, and the time taken to generate each axes is the mean of how long it took across 30 times.