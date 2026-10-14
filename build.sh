#!/usr/bin/bash



run_benchmark(){
	for name in "$@"
	do
		target="benchmarks/bench_${name}"
		build="benchmarks/${name}_benchmark.c"
		gcc -O2  -I.. -Wall -Wextra  $build -L. -ldtypes -o $target


	done

	for name in "$@"
	do
		target="benchmarks/bench_${name}"
		$target 
		[ -n "$PURGE" ] && rm -f $target
	done



}


main(){
	gcc -O2 -pedantic  -Wall -Wextra -c -I.. *.c 
	ar rcs libdtypes.a *.o
	rm *.o

	[  -n "$BENCH"  ] || exit 0
	lscpu | head -n 8 2>&1 
       	run_benchmark array hashmap hashset lookup trie deque bloom bitset ndarray

	# [  -n "$BENCH"  ] && run_benchmark ndarray
}
set -e
main

