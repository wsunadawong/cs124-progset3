#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
// #include "heap.h"
// #include "solution.h"

#define MAX_ITER 25000

int* sequenceGenerate(int n) {
	// Seed RNG
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned) tv.tv_usec);

    // Allocate space for sequence
	int* s;

	// Assign randomly to sets
	for (int i = 0; i < n; i++) {
		int set = rand() & 1;
		s[i] = (set == 1) ? 1 : -1;
	}

	return s;
}

// Produces a random neighbor of a sequence
// Returns mutated sequence
int* sequenceNeighbor(int* s, int n) {
	// Produce neighbor
	int* neighbor;
	for (int i = 0; i < n; i++) {
		neighbor[i] = s[i];
	}

	// Change sequence
	int i = rand() % n;
	neighbor[i] = -neighbor[i];

	// Change second one with probability 0.5
	if ((double) rand() / RAND_MAX > 0.5) {
		int j = i;
		while (j == i) {
			j = rand() % n;
		}
		neighbor[j] = -neighbor[j];
	}

	return neighbor;
}

// Generates a random prepartitioning (using [0, n))
// NOTE: need to free returned array
int* partitionGenerate(int n) {
	// Seed RNG
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((unsigned) tv.tv_usec);

    int* s;

    // Assign randomly to partition
    for (int i = 0; i < n; i++) {
    	s[i] = rand() % n;
    }

    return s;
}

// Produces a random neighbor of a sequence
// Returns mutated sequence
int* partitionNeighbor(int* s, int n) {
	// Produce neighbor
	int* neighbor;
	for (int i = 0; i < n; i++) {
		neighbor[i] = s[i];
	}

	// Produce neighbor
	int i = rand() % n;
	int j = neighbor[i];

	// Ensure that different number gets assigned
	while (j == neighbor[i]) {
		j = rand() % n;
	}

	neighbor[i] = j;

	return neighbor;
}

// Converts a sequence solution into a partition solution
int* sequenceToPartition(int* s, int n) {
	int* p;
	for (int i = 0; i < n; i++) {
		p[i] = (s[i] == 1) ? 1 : 0;
	}
	return p;
}

// Prints set passed in, whether solution or prepartition
void printSolution(int* s, int n) {
	for (int i = 0; i < n; i++) {
		printf("%4i ", s[i]);
	}
	printf("\n");
}

// Calculates residue for a sequence solution
long seqRes(long* nums, int* sign, int n) {
    long residue = 0;
    for (int i = 0; i < n; i++) {
        residue += sign[i] * nums[i];
    }
    return residue;
}

// Calculates residue for a prepartition solution
long partitionResidue(long* nums, int* s, int n) {
    // Calculate new numbers based on prepartition
    long* newNums;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (s[j] == i) {
                newNums[i] += nums[j];
            }
        }
    }

    // Get residue
    long residue = kk(newNums, n);
    free(newNums);
    return residue;
}

// Uses repeated random to find a solution to number partition
long repeatedRandom(long* nums, int* start, int n, int isSequence) {
    // Keep track of s; can't use passed in pointer (mutable)
    int* s;
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
    }

    if (isSequence) {
        uint64_t bestResidue = seqRes(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* randomSolution = sequenceGenerate(n);
            uint64_t residue = seqRes(nums, randomSolution, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = randomSolution;
            } else {
                free(randomSolution);
            }

        }
        free(s);
        return bestResidue;
    } else {
        uint64_t bestResidue = partitionResidue(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* randomSolution = partitionGenerate(n);
            uint64_t residue = partitionResidue(nums, randomSolution, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = randomSolution;
            } else {
                free(randomSolution);
            }

                        // Write results
            // FILE* f = fopen("data/rrp.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open rrp.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu\n", i, bestResidue, residue);
            // fclose(f);
        }
        free(s);
        return bestResidue;
    }
}

// Uses hill climbing to find a solution
long hillClimbing(long* nums, int* start, int n, int isSequence) {
    // Keep track of s; can't use passed in pointer (mutable)
    int* s;
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
    }

    if (isSequence) {
        long bestResidue = seqRes(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = sequenceNeighbor(s, n);
            uint64_t residue = seqRes(nums, neighbor, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Write results
            // FILE* f = fopen("data/hcs.csv", "a");
            // if (f == NULL) {
            //     printf("Could not open hcs.csv\n");
            //     return -1;
            // }
            // fprintf(f, "%i,%llu,%llu\n", i, bestResidue, residue);
            // fclose(f);
        }
        free(s);
        return bestResidue;
    } else {
        uint64_t bestResidue = partitionResidue(nums, s, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Obtain random solution
            int* neighbor = partitionNeighbor(s, n);
            uint64_t residue = partitionResidue(nums, neighbor, n);

            // If better, replace
            if (residue < bestResidue) {
                bestResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }
        }
        free(s);
        return bestResidue;
    }
}

// Cooling function for simulated annealing
double cooling(int iter) {
    return pow(10, 10) * pow(0.8, iter / 300);
}

// Uses simulated annealing to find a solution
long simulatedAnnealing(long* nums, int* start, int n, int isSequence) {
    // Keep track of s and bestS; can't use passed in pointer (mutable)
    int* s;
    int* bestS;
    for (int i = 0; i < n; i++) {
        s[i] = start[i];
        bestS[i] = start[i];
    }


    if (isSequence) {
        long sResidue = seqRes(nums, s, n);
        long bestResidue = seqRes(nums, bestS, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = sequenceNeighbor(s, n);
            long residue = seqRes(nums, neighbor, n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((long) (sResidue - residue) / cooling(i))) {
                sResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Reassign best solution
            if (sResidue < bestResidue) {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) {
                    bestS[j] = s[j];
                }
            }


        }
        free(s);
        free(bestS);
        return bestResidue;
    } else {
        long sResidue = partitionResidue(nums, s, n);
        long bestResidue = partitionResidue(nums, bestS, n);
        for (int i = 0; i < MAX_ITER; i++) {
            // Get neighbor
            int* neighbor = partitionNeighbor(s, n);
            long residue = partitionResidue(nums, neighbor, n);

            // If better or within probability, replace
            if (residue < sResidue || (double) rand() / RAND_MAX < exp((int64_t) (sResidue - residue) / cooling(i))) {
                sResidue = residue;
                free(s);
                s = neighbor;
            } else {
                free(neighbor);
            }

            // Reassign best solution
            if (sResidue < bestResidue) {
                bestResidue = sResidue;
                for (int j = 0; j < n; j++) {
                    bestS[j] = s[j];
                }
            }
        }
        free(s);
        free(bestS);
        return bestResidue;
    }
}