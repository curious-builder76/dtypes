#ifndef DTYPES_PRINT_H

#define DTYPES_PRINT_H 

#include<stdio.h>

int print_benchmark(char* name,size_t operations, double time_elapsed){
	fflush(stdout);
	printf(
                        "%s\n"
                        "  Time: %.6f seconds\n"
                        "  Operations: %zu\n"
                        "  Rate: %.2f M ops/sec\n\n",
			name,
                        time_elapsed,
                        operations,
                        (operations / time_elapsed) / 1000000.0
              );
	fflush(stdout);
	return 0;
}
#endif
