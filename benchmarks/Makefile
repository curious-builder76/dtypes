run:
	gcc -I../.. hashmap_benchmark.c -O2 -Wextra -Wall -o bench_hashmap
	gcc -I../.. hashset_benchmark.c  -O2 -Wextra -Wall -o bench_hashset
	gcc -I../.. array_benchmark.c  -O2 -Wextra -Wall -o bench_array
	gcc -I../.. lookup_benchmark.c  -O2 -Wextra -Wall -o bench_lookup
	./bench_hashset
	./bench_hashmap
	./bench_array
	./bench_lookup
