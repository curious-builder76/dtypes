#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "bloom.h"

#define CAPACITY    10000000
#define ELEMENTS    1000000
#define ITERATIONS  10

typedef struct {
    uint64_t value;
} item_t;


/*
 * Two deterministic 64-bit hash functions.
 */

static uint64_t hash1(void *data, size_t size)
{
    uint8_t *p = data;
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < size; i++) {
        hash ^= p[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}

static uint64_t hash2(void *data, size_t size)
{
    uint8_t *p = data;
    uint64_t hash = 0xcbf29ce484222325ULL;

    for (size_t i = 0; i < size; i++) {
        hash ^= p[i];
        hash *= 0x100000001b3ULL;
        hash ^= hash >> 32;
    }

    return hash;
}


static double elapsed(
    struct timespec *start,
    struct timespec *end
)
{
    return (double)(end->tv_sec - start->tv_sec)
         + (double)(end->tv_nsec - start->tv_nsec) / 1e9;
}


int main(void)
{
    struct timespec start, end;

    printf("==== benchmark of bloom filter ====\n");
    printf("Capacity: %d bits\n", CAPACITY);
    printf("Elements: %d\n", ELEMENTS);
    printf("Iterations: %d\n\n", ITERATIONS);


    bloom_t *bloom = bloom_new(
        CAPACITY,
        sizeof(item_t),
        hash1,
        hash2
    );

    if (bloom == NULL) {
        fprintf(stderr, "failed to create bloom filter\n");
        return 1;
    }


    /*
     * Generate deterministic data.
     */
    item_t items[ELEMENTS];

    for (uint64_t i = 0; i < ELEMENTS; i++)
        items[i].value = i;


    /*
     * PUT
     */

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int iteration = 0;
         iteration < ITERATIONS;
         iteration++) {

        for (size_t i = 0; i < ELEMENTS; i++)
            bloom_add(bloom, &items[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double put_time = elapsed(&start, &end);

    uint64_t put_ops =
        (uint64_t) ELEMENTS * ITERATIONS;

    printf("PUT\n");
    printf("  Time: %.6f seconds\n", put_time);
    printf("  Operations: %llu\n",
           (unsigned long long)put_ops);
    printf("  Rate: %.2f M ops/sec\n\n",
           put_ops / put_time / 1000000.0);


    /*
     * GET
     *
     * Every item was inserted, so a correct Bloom filter
     * should report all of these as present.
     */

    size_t found = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int iteration = 0;
         iteration < ITERATIONS;
         iteration++) {

        for (size_t i = 0; i < ELEMENTS; i++) {

            if (bloom_contains(bloom, &items[i]))
                found++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double get_time = elapsed(&start, &end);

    uint64_t get_ops =
        (uint64_t) ELEMENTS * ITERATIONS;

    printf("GET (inserted)\n");
    printf("  Time: %.6f seconds\n", get_time);
    printf("  Operations: %llu\n",
           (unsigned long long)get_ops);
    printf("  Rate: %.2f M ops/sec\n",
           get_ops / get_time / 1000000.0);
    printf("  Positive results: %llu\n\n",
           (unsigned long long)found);


    /*
     * FALSE POSITIVE TEST
     *
     * These values were never inserted.
     */

    size_t false_positives = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < ELEMENTS; i++) {

        item_t item = {
            .value = (uint64_t) ELEMENTS + i
        };

        if (bloom_contains(bloom, &item))
            false_positives++;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double fp_time = elapsed(&start, &end);

    double fp_rate =
        (double) false_positives
        / ELEMENTS
        * 100.0;

    printf("FALSE POSITIVE TEST\n");
    printf("  Time: %.6f seconds\n", fp_time);
    printf("  Tested: %d\n", ELEMENTS);
    printf("  False positives: %llu\n",
           (unsigned long long)false_positives);
    printf("  False-positive rate: %.4f%%\n\n",
           fp_rate);


    bloom_destroy(bloom);

    return 0;
}
