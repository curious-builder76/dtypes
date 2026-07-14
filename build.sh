#!/usr/bin/bash



run_benchmark(){
	for name in "$@"
	do
		target="benchmarks/bench_${name}"
		build="benchmarks/${name}_benchmark.c"
		gcc  -ggdb -O2 -I.. -Wall -Wextra $build -L. -ldtypes -o $target
	done

	for name in "$@"
	do
		target="benchmarks/bench_${name}"
		$target 
		[ -n "$PURGE" ] && rm -f $target
	done



}


main(){
	gcc -Wall -Wextra -c -I.. *.c 
	ar rcs libdtypes.a *.o
	rm *.o
	# [  -n "$BENCH"  ] && run_benchmark array hashmap hashset lookup trie
	[  -n "$BENCH"  ] && run_benchmark deque 
}
set -e
main

