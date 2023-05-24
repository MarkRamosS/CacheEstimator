#include <cstdint>
#include <sstream>
#include <fstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Buckets in csv files of input
#define BUCKETS buckets.size()
#define MAX_RD	(4227858433) // All buckets
// COLD_MISSES activates the code which takes into consideration cold misses 
// when calculating reuse distance scaling or stack distances
// #define COLD_MISSES

#define SMALLEST_CACHE (32 * 1024)
#define LARGEST_CACHE  (8192 * 1024)



//--------->          Interface Functions Prototypes         <--------------
double statcache_random_solver (double L);
double statcache_lru_solver (double L);

//--------->           Backend Functions Prototypes          <--------------
static void initialization (int argc, char **argv);
static double f2(double r, double L);
static double statcache_lru_calc_unique_occurancies (unsigned reuse_distance);

//--------->                 Global Variables                <--------------
vector<uint64_t> buckets, diffs, histogram, cumul_histogram;
unsigned max_rd	      = 0;		    // Highest defined reuse distance
double total_accesses = 0.0;		// The number of accesses in the histogram
/*
 * This function computes the sum of the miss probabilities of all sampled 
 * memory references, same as the right-hand side of eq. 5 in the ISPASS 
 * paper. 
 * r is the miss ratio, called 'R' in the paper
 * L is the number of cache lines
 * histogram is a vector of reuse distances
 * hist_size is the length of histogram
 */

static double f2(double r, double L) 
{
	uint64_t rd; 
	double sum = 0.0;

	double factor;
	double hit_prob = 1.0;

	// Optimizations
	// 1) hit probability does not require one pow for each iteration
	//    Instead we calculate the factor by which the hit probability changes between 
    //    iterations
	// 2) If hit probability goes below 10^-6, 
	//    accesses above that reuse distance are considered certain misses
	factor = (L - 1.0) / L;
	factor = pow(factor, r);

	for (rd = 0; ((rd < BUCKETS) && (hit_prob > 0.000001)); rd++)
	{
		hit_prob = pow(factor, buckets[rd]);
		sum += (double)histogram[rd] * (1.0 - hit_prob);
	}
	
	sum += (double)(total_accesses - cumul_histogram[rd-1]);
	
	return sum;
}
/* Returns the miss ratio. 
 * This is a solver for eq. 5 (ISPASS) 
 * histogram holds the Histogram Data
 * L is the number of cache lines in the cache (cache size / cache line size)
 * The miss ratio is found by bisection.
 */
double statcache_random_solver (double L)
{
	double rmax = 0.9999;	// This corresponds to 100% miss ratio
	double rmin = 0.0001;	// This corresponds to 0%   miss ratio
	double rmid = 0.0;

	double left, right;

	if (f2(rmax, L) >= total_accesses * rmax)
	       return 1.0;

	if (f2(rmin, L) < total_accesses * rmin)
		return 0.0;
  
	while ((rmax - rmin) > 0.0001) 
	{
		rmid = (rmax + rmin)/2;
    
		left  = f2 (rmid, L);
		right = rmid * total_accesses;

		if (left < right)
			rmax = rmid;
		else
			rmin = rmid;
	} 
	rmid = (rmax + rmin)/2 ;

	return rmid;
}

/****************************************************
 *                  LRU SOLVER                      *
 ****************************************************
 */

/* Returns the miss ratio. 
 * histogram holds the Histogram Data
 * L is the number of cache lines in the cache (cache size / cache line size)
 */
double statcache_lru_solver (double L)
{
	int i;

	unsigned low_limit  = 0;
    //while(buckets[low_limit] < L) low_limit++;
	unsigned high_limit = BUCKETS-1;
	unsigned middle_limit;

	double hits = 0.0;
	double misses = 0.0;

	if (statcache_lru_calc_unique_occurancies(high_limit) <= L)
		return 0.0;
	
	if (statcache_lru_calc_unique_occurancies(low_limit) >= L)
		return 1.0;
	
	while (high_limit - low_limit > 1)
	{
		middle_limit = (low_limit + high_limit) / 2;

		if (statcache_lru_calc_unique_occurancies (middle_limit) > L)
			high_limit = middle_limit;
		else
			low_limit = middle_limit;
	}

	hits = (double) cumul_histogram[low_limit];
	misses = total_accesses - hits;

	return (misses / total_accesses);
}

static double statcache_lru_calc_unique_occurancies (unsigned reuse_distance)
{
	uint32_t i = 0;
	uint64_t sum = 0;

	for (i = 0; i <= reuse_distance; i++)
		sum += (buckets[i]) * histogram[i];

    sum += buckets[reuse_distance] * (uint64_t)(total_accesses - cumul_histogram[reuse_distance]);
    // cout<<"MILK:"<<((double)sum)/total_accesses<<endl;
    return ((double)sum)/total_accesses;
}

// Input: CSV file with:
// First line: bucket indices (0,1,2,...)
// Next lines: For each bucket a reuse distance value
void initialization(int argc, char** argv){
    vector<vector<uint64_t>> content;
	vector<uint64_t> row;
	string line, word, fname;
    uint64_t tmp;
    stringstream ss;
    fname = argv[1];
	fstream file(fname, ios::in);
    if(file.is_open())
    {
        row.clear();

        getline(file, line);
        stringstream str(line);

        bool tmp_ctr = true;
        while(getline(str, word, ',')){
            if(tmp_ctr){
                tmp_ctr = false;
                continue;
            }
            ss.clear();
            ss << word;
            ss >> tmp;
            if(tmp >= MAX_RD) 
                break;
            if(buckets.size() > 0){
                diffs.push_back(tmp - buckets[buckets.size()-1]);
            } else {
                diffs.push_back(1);
            }
            buckets.push_back(tmp);
        }
        content.push_back(diffs);
        content.push_back(buckets);
        row.clear();
        if(diffs.size() != BUCKETS){
            cout<<"diffs size: "<<diffs.size()<<endl;
        }
        // cout<<"Buckets: "<<buckets.size()<<endl;
        printf("random_32,lru_32");
        printf("random_64,lru_64");
        printf("random_128,lru_128");
        printf("random_256,lru_256");
        printf("random_512,lru_512");
        printf("random_1024,lru_1024");
        printf("random_2048,lru_2048");
        printf("random_4096,lru_4096");
        printf("random_8192,lru_8192\n");

        while(getline(file, line)){
            stringstream str2(line);
            histogram.clear();
            cumul_histogram.clear();
            bool tmp_ctr = true;
            while(getline(str2, word, ',')){
                if(tmp_ctr){
                    tmp_ctr = false;
                    continue;
                }
                ss.clear();
                ss << word;
                ss >> tmp;
                row.push_back(tmp);
                if(histogram.size() == buckets.size())
                    break;
                if(histogram.size() == 0)
                    cumul_histogram.push_back(tmp);
                else
                    cumul_histogram.push_back(cumul_histogram[cumul_histogram.size()-1] + tmp);
                histogram.push_back(tmp);
            }
            if(histogram.size() != BUCKETS){
                cout<<"histogram size: "<<histogram.size()<<endl;
            }
            if(cumul_histogram.size() != BUCKETS){
                cout<<"cumul_histogram size: "<<cumul_histogram.size()<<endl;
            }
            content.push_back(histogram);
            content.push_back(cumul_histogram);
            total_accesses = cumul_histogram[cumul_histogram.size()-1];
            int cache_size;
            int cache_size_chkpt;
            double miss_rate_random, miss_rate_lru;
            cache_size       = SMALLEST_CACHE;
            cache_size_chkpt = cache_size * 2;

            while (cache_size <= LARGEST_CACHE)
            {
                if(cache_size != SMALLEST_CACHE)
                    printf(",");
                miss_rate_random = statcache_random_solver((double) (cache_size / 64));
                miss_rate_lru    = statcache_lru_solver((double) (cache_size / 64));

                printf ("%lf,%lf", miss_rate_random, miss_rate_lru);

                cache_size *= 2;
                if (cache_size >= cache_size_chkpt)
                {
                    cache_size = cache_size_chkpt;
                    cache_size_chkpt *= 2;
                }
            }
            printf("\n");
        }
    }
    else
    {
        cout<<"Could not open the file\n";
        exit(0);
    }
    for(int i=0;i<content.size();i++)
    {
        for(int j=0;j<content[i].size();j++)
        {
            //cout<<content[i][j]<<" ";
        }
        //cout<<"\n DONE \n";
    }
}

// Input: filename
int main (int argc, char **argv)
{
	// Initialization sets all the global variables and loads the histograms
	// If not used as a standalone program, it should be probably removed
	initialization(argc, argv);

	return 0;
}
