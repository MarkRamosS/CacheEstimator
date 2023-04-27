#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


// MAX_RD holds the maximum reuse distance (+1) stored in the histogram
// Increase it for larger histograms
#define MAX_RD	(200 * 1024 * 1024)
// COLD_MISSES activates the code which takes into consideration cold misses 
// when calculating reuse distance scaling or stack distances
// #define COLD_MISSES

#define SMALLEST_CACHE (32 * 1024)
#define LARGEST_CACHE  (8192 * 1024)


//--------->          Interface Functions Prototypes         <--------------
double statcache_random_solver (unsigned *histogram, unsigned hist_size, double L);
double statcache_lru_solver (unsigned *histogram, unsigned hist_size, double L);

//--------->           Backend Functions Prototypes          <--------------
static void initialization (int argc, char **argv);
static double f2(double r, double L, unsigned *histogram, unsigned hist_size);
static double statcache_lru_calc_unique_occurancies (unsigned *histogram, unsigned hist_size, unsigned reuse_distance);

//--------->                 Global Variables                <--------------
unsigned *histogram;			// The input histogram
long long unsigned *cumul_histogram;	// The cumulative histogram, needed for algorithm optimizations

unsigned max_rd	      = 0;		// Highest defined reuse distance
double total_accesses = 0.0;		// The number of accesses in the histogram
#ifdef COLD_MISSES
double cold_misses    = 0.0;		// Cold misses number
#endif

	
int main (int argc, char **argv)
{
	int cache_size;
	int cache_size_chkpt;
	double miss_rate_random, miss_rate_lru;
	// Initialization sets all the global variables and loads the histograms
	// If not used as a standalone program, it should be probably removed
	initialization(argc, argv);

	cache_size       = SMALLEST_CACHE;
	cache_size_chkpt = cache_size * 2;

	while (cache_size <= LARGEST_CACHE)
	{
   //     printf("Cache Size: %d", cache_size);
		miss_rate_random = statcache_random_solver (histogram, (max_rd + 1), (double) (cache_size / 64));
		miss_rate_lru    = statcache_lru_solver (histogram, (max_rd + 1), (double) (cache_size / 64));

		printf ("%d\t%lf\t%lf\n", cache_size / 1024, miss_rate_random, miss_rate_lru);

		cache_size *= 1.04;
		if (cache_size >= cache_size_chkpt)
		{
			cache_size = cache_size_chkpt;
			cache_size_chkpt *= 2;
		}
	}
	return 0;
}

static void initialization (int argc, char **argv)
{
	int rd;
	unsigned hist_value;

#ifdef COLD_MISSES
	if (argc < 2)
		exit(0);

	cold_misses = (double) strtod (argv[1], NULL);
#endif
	
	histogram       = (unsigned *) calloc(MAX_RD, sizeof(unsigned));
	cumul_histogram = (long long unsigned *) calloc(MAX_RD, sizeof(long long unsigned));
    printf("Here.");
    scanf ("%*[^\n]\n");
	while (scanf ("%d,%d,%*d\n", &rd, &hist_value) == 2)
	{
        //printf("RD: %d \n", rd);
		if (rd >= MAX_RD)
			break;

		total_accesses     += hist_value;
		histogram[rd]       = hist_value;
	}
	max_rd = rd;
    printf("And here.");
	cumul_histogram[0] = histogram[0];
	for (rd = 1; rd <= max_rd; rd++)
		cumul_histogram[rd] = cumul_histogram[rd - 1] + histogram[rd];
}


/* Returns the miss ratio. 
 * This is a solver for eq. 5 (ISPASS) 
 * histogram holds the Histogram Data
 * L is the number of cache lines in the cache (cache size / cache line size)
 * The miss ratio is found by bisection.
 */
double statcache_random_solver (unsigned *histogram, unsigned hist_size, double L)
{
	double rmax = 0.9999;	// This corresponds to 100% miss ratio
	double rmin = 0.0001;	// This corresponds to 0%   miss ratio
	double rmid = 0.0;

	double left, right;

	if (f2(rmax, L, histogram, hist_size) >= total_accesses * rmax)
	       return 1.0;

	if (f2(rmin, L, histogram, hist_size) < total_accesses * rmin)
		return 0.0;
  
	while ((rmax - rmin) > 0.0001) 
	{
		rmid = (rmax + rmin)/2;
    
		left  = f2 (rmid, L, histogram, hist_size);
		right = rmid * total_accesses;

		if (left < right)
			rmax = rmid;
		else
			rmin = rmid;
	} 
	rmid = (rmax + rmin)/2 ;

	return rmid;
}


/*
 * This function computes the sum of the miss probabilities of all sampled 
 * memory references, same as the right-hand side of eq. 5 in the ISPASS 
 * paper. 
 * r is the miss ratio, called 'R' in the paper
 * L is the number of cache lines
 * histogram is a vector of reuse distances
 * hist_size is the length of histogram
 */

static double f2(double r, double L, unsigned *histogram, unsigned hist_size) 
{
	int rd; 
	double sum = 0.0;

	double factor;
	double hit_prob = 1.0;

#ifdef COLD_MISSES
	r = (r * total_accesses  + cold_misses ) / (total_accesses + cold_misses);
#endif

	// Optimizations
	// 1) hit probability does not require one pow for each iteration
	//    Instead we calculate the factor by which the hit probability changes between iterations
	// 2) If hit probability goes below 10^-6, 
	//    accesses above that reuse distance are considered certain misses
	factor = (L - 1.0) / L;
	factor = pow(factor, r);

	for (rd = 1; ((rd < hist_size) && (hit_prob > 0.000001)); rd++)
	{
		hit_prob *= factor;
		sum += (double)histogram[rd] * (1.0 - hit_prob);
		
	}
	
	sum += (double)(total_accesses - cumul_histogram[rd-1]);
	
	return sum;
}

/****************************************************
 *                  LRU SOLVER                      *
 ****************************************************
 */

/* Returns the miss ratio. 
 * histogram holds the Histogram Data
 * L is the number of cache lines in the cache (cache size / cache line size)
 */
double statcache_lru_solver (unsigned *histogram, unsigned hist_size, double L)
{
	int i;

	unsigned low_limit  = L;
	unsigned high_limit = hist_size;
	unsigned middle_limit;

	double hits = 0.0;
	double misses = 0.0;

	if (statcache_lru_calc_unique_occurancies(histogram, hist_size, high_limit) <= L)
		return 0.0;
	
	if (statcache_lru_calc_unique_occurancies(histogram, hist_size, low_limit) >= L)
		return 1.0;
	
	while ((high_limit - low_limit > 1))
	{
		middle_limit = (low_limit + high_limit) / 2;

		if (statcache_lru_calc_unique_occurancies (histogram, hist_size, middle_limit) > L)
			high_limit = middle_limit;
		else
			low_limit = middle_limit;
	}

	middle_limit = low_limit;

	hits = (double) cumul_histogram[low_limit];
	misses = total_accesses - hits;

	return (misses / total_accesses);
}

static double statcache_lru_calc_unique_occurancies (unsigned *histogram, unsigned hist_size, unsigned reuse_distance)
{
	unsigned i;
	long long unsigned sum = 0;

	for (i = 1; i <= reuse_distance; i++)
		sum += i * (long long unsigned)histogram[i];

	sum += reuse_distance * (long long unsigned)(total_accesses - cumul_histogram[reuse_distance]);

#ifdef COLD_MISSES
	sum += reuse_distance * (long long unsigned)cold_misses;
	return ((double)sum)/(total_accesses + cold_misses);
#else
	return ((double)sum)/total_accesses;
#endif

}

