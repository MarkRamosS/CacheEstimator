# CacheEstimator

A project aiming to detect cache misses of certain programs based on the compiled source code and the reuse distance histograms.

## Simulation 
For the Simulation we've used the Champsim tool (https://github.com/ChampSim/ChampSim). This returns the simulated cache accesses, hits and misses we'll use as the ground truth of our program. Since the Stat Cache paper mentions it, we need the ground truth every 200k accesses to split the program in small (almost stable) data pieces. So there has been a little modification to print the simulated cache's state every 2 million iterations, so approximately every 200k accesses. 

## Reuse Distance Calculation 
Reuse Distance is calculated with help of rdprof. A tool added to the replacement policy lru to create the rdlru. This creates a histogram of all the reuse distances that go through the cache it is used on. We've added it to the L1D to detect all distances even those from the above caches (L2C, LLC). This prints it's state every 2 million accesses, together with the ground truth.


## Reuse Distances and Cache Simulations
The outputs of the execution are located in the database repository: https://github.com/MarkRamosS/ce_data




