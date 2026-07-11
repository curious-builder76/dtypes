#!/usr/bin/bash



run_benchmark(){
	name=$1

	target="benchmarks/bench_${name}"
	build="benchmarks/${name}_benchmark.c"

	gcc  -O2 -I.. -Wall -Wextra $build -L. -ldtypes -o $target
	$target
	rm $target

}


main(){
	gcc -Wall -Wextra -c -I.. *.c
	ar rcs libdtypes.a *.o
	rm *.o
	run_benchmark array
	run_benchmark hashmap
	run_benchmark lookup
	run_benchmark hashset
	run_benchmark trie
}
set -e
main

