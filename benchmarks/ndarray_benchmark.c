#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dtypes/ndarray.h"

#define SIZE 100
#define ITERATIONS 10

static double elapsed(
		clock_t start,
		clock_t end
		)
{
	return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(void)
{
	ndarray_t* array = ndarray_new(
			sizeof(int),
			3,
			SIZE,
			SIZE,
			SIZE
			);
	if (array == NULL)
	{
		fprintf(stderr, "Failed to create ndarray\n");
		return 1;
	}

	puts("====benchmark of ndarray====");
	size_t total = (size_t)SIZE * SIZE * SIZE;

	printf(
			"Array: %d x %d x %d\n"
			"Elements: %zu\n"
			"Iterations: %d\n\n",
			SIZE,
			SIZE,
			SIZE,
			total,
			ITERATIONS
	      );


	/* Benchmark put */

	volatile long long valid_checksum=0;
	clock_t start = clock();
	for (size_t iteration = 0;
			iteration < ITERATIONS;
			iteration++)
	{
		for (size_t i = 0; i < SIZE; i++)
		{
			for (size_t j = 0; j < SIZE; j++)
			{
				for (size_t k = 0; k < SIZE; k++)
				{
					int value =
						(int)(i * SIZE * SIZE +
								j * SIZE +
								k);
					valid_checksum+=value;
					ndarray_put(
							array,
							&value,
							i,
							j,
							k
						   );
				}
			}
		}
	}

	clock_t end = clock();

	double put_time = elapsed(start, end);

	printf(
			"PUT\n"
			"  Time: %.6f seconds\n"
			"  Operations: %zu\n"
			"  Rate: %.2f M ops/sec\n\n",
			put_time,
			total * ITERATIONS,
			(total * ITERATIONS / put_time) / 1000000.0
	      );


	/* Benchmark get */

	volatile long long sum = 0;

	start = clock();

	for (size_t iteration = 0;
			iteration < ITERATIONS;
			iteration++)
	{
		for (size_t i = 0; i < SIZE; i++)
		{
			for (size_t j = 0; j < SIZE; j++)
			{
				for (size_t k = 0; k < SIZE; k++)
				{

					int* value = ndarray_get(
							array,
							i,
							j,
							k
							);
					sum += *value;
				}
			}
		}
	}

	end = clock();

	double get_time = elapsed(start, end);

	printf(
			"GET\n"
			"  Time: %.6f seconds\n"
			"  Operations: %zu\n"
			"  Rate: %.2f M ops/sec\n\n",
			get_time,
			total * ITERATIONS,
			(total * ITERATIONS / get_time) / 1000000.0
	      );

	/* Prevent compiler from optimizing everything away */

	printf("Checksum: %lld\n", sum);
	printf("Expected: %lld\n", valid_checksum);
	if(sum!=valid_checksum){
		printf("FAILED!!!\n");
	}
	else{
		printf("Passed.\n");
	}
	ndarray_destroy(array);

	return 0;
}

