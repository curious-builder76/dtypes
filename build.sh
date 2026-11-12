#!/usr/bin/env bash
set -euo pipefail

[[ -v "CC" ]] || CC="gcc"
CFLAGS="${CFLAGS:-} -Wall -Wextra -Wpedantic -I."

genflags() {
  printf '%s\n' $CFLAGS > compile_flags.txt
}

benchmark() {
	for name in "$@"; do
		target="benchmarks/bench_${name}"
		build="benchmarks/${name}_benchmark.c"
		${CC} ${CFLAGS} -O2 $build -L. -ldtypes -o $target
	done

	for name in "$@"; do
		target="benchmarks/bench_${name}"
		$target
		if [[ -v PURGE ]]; then
			rm -f $target
		fi
	done
}

main() {
	${CC} -O2 ${CFLAGS} -c *.c
	ar rcs libdtypes.a *.o
	rm *.o

	if [[ -v BENCH ]]; then
		lscpu | head -n 8 2>&1
	 	benchmark array hashmap hashset lookup trie deque bloom bitset ndarray list
	fi

	if [[ -v GFLAG ]]; then
		genflags
	fi
}

main
