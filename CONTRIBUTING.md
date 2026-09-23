# Contributing to dtypes

Thanks for your interest in contributing to **dtypes**!

dtypes is a lightweight collection of generic data structures written in C. The project focuses on simple APIs, minimal dependencies, reusable implementations, custom allocators, and efficient memory usage.

Contributions are welcome, whether they are bug fixes, improvements, documentation, tests, benchmarks, or new data structures.

## Before You Start

Please read:

- `README.md` — project overview, API conventions, and build instructions
- `CODE_OF_CONDUCT.md` — community guidelines
- The relevant `.h` and `.c` files for the data structure you intend to modify

The project is still evolving, so APIs and internal implementations may change.

## What You Can Contribute

Some useful areas include:

- Bug fixes
- Memory-management fixes
- API improvements
- New data structures
- Improvements to existing data structures
- Custom allocator support
- Performance improvements
- Benchmarks
- Tests
- Documentation
- Build-system improvements
- Portability improvements
- Windows compatibility
- Code cleanup and refactoring

If you are unsure whether an idea fits the project, open an issue first and describe what you want to change.

## Repository Structure

Most data structures follow a simple pair of files:

```text
array.c
array.h

hashmap.c
hashmap.h

hashset.c
hashset.h

lookup.c
lookup.h

trie.c
trie.h
```
Benchmarks are kept under:
```text
benchmarks/
```
The implementation should generally remain in the corresponding .c file, while public declarations belong in the .h file.

API Conventions

dtypes aims to keep its APIs consistent across data structures.

A typical data structure exposes functions following this pattern:
```C
*_custom()
*_new()
*_destroy()

*_custom()
```
Used when the caller needs more control over configuration, including custom allocation and deallocation where supported.

`*_new()`

The simpler constructor intended for normal use.

`*_destroy()`

Releases resources owned by the data structure.

When adding a new data structure, follow the existing naming conventions as closely as possible.

For example:
```C
array_t *array_new(...);
array_t *array_custom(...);
void array_destroy(array_t *array);
```
Avoid introducing a completely different API style unless there is a strong reason to do so.

Memory Management

Memory management is an important part of dtypes.

When modifying or adding a data structure:

Check allocations for failure where appropriate.

Make ownership clear.

Ensure every owned allocation has a corresponding cleanup path.

Do not introduce memory leaks.

Do not free memory that the data structure does not own.

Preserve custom allocator/deallocator support where the API provides it.

Be careful with integer overflow when calculating allocation sizes.

Consider zero-sized and unusually large allocations where relevant.


If a structure uses contiguous memory intentionally, avoid replacing it with a node-per-allocation design without a clear reason.

Error Handling

Public APIs should fail predictably.

When an operation can fail:

Follow the existing error convention used by that data structure.

Avoid silently continuing after allocation or internal-operation failures.

Do not introduce undefined behavior as an error-handling mechanism.

Document new failure cases when they are observable by users.


Do not change an existing function's return semantics without considering API compatibility.

Performance

Performance matters, but correctness comes first.

Before optimizing:

1. Identify the actual bottleneck.


2. Create or update a benchmark demonstrating the behavior.


3. Make the change.


4. Compare the results.


5. Verify that the optimization does not introduce correctness or memory-management problems.



Avoid optimizations based only on assumptions.

If a performance change is significant, include benchmark results in the pull request.

Example:

Before:
PUT: 0.42 s
GET: 0.18 s

After:
PUT: 0.31 s
GET: 0.15 s

Mention the environment and compiler options when the numbers matter.

Testing

Before submitting a pull request, test the affected data structure thoroughly.

At minimum, test:

Normal usage

Empty structures

Single-element structures

Multiple elements

Duplicate values where applicable

Missing values

Boundary conditions

Allocation failures where practical

Destruction and cleanup

Custom allocators when applicable


For data structures involving indexing, sizes, capacities, or offsets, pay particular attention to:

Integer overflow

Off-by-one errors

Out-of-bounds access

Invalid dimensions

Zero-sized dimensions


For pointer-heavy structures such as tries and linked structures, also test unusual insertion and deletion orders.

Building

Follow the build instructions in the README.

For a normal build, use the project's existing build system rather than introducing a separate build procedure.

If you modify the build system, make sure the normal build still works after your changes.

If your change affects portability, test on the platforms you have access to and clearly state what you tested.

Adding a New Data Structure

Before implementing a new data structure, open an issue if the addition is substantial.

A new data structure should generally include:

<name>.h

<name>.c

Appropriate documentation

Tests or demonstrable usage

A benchmark when performance is relevant


The API should be generic and consistent with the rest of dtypes.

For example:

foo.h
foo.c

should expose:

foo_t
foo_new()
foo_custom()
foo_destroy()

where those operations make sense for the structure.

Avoid adding a data structure simply because it is possible to implement it. It should provide a useful addition to the library.

Documentation

Update the README when your change affects:

Public APIs

Installation

Build instructions

Supported platforms

Available data structures

Important behavior

Usage examples


Public functions should have sufficiently clear names and declarations that their intended use is understandable.

If behavior is non-obvious, document it.

Code Style

Keep the code consistent with the surrounding implementation.

In particular:

Use clear names.

Keep public APIs simple.

Avoid unnecessary dependencies.

Avoid unrelated refactoring in the same pull request.

Prefer straightforward C over unnecessarily clever implementations.

Keep changes focused.


Do not reformat an entire source file merely to change a small section.

Pull Requests

A good pull request should contain one focused change.

Please include:

What changed

Why it changed

How it was tested

Benchmark results when relevant

Any compatibility or API changes


A useful pull request description might look like:

## What changed

Fixed an out-of-bounds access in the trie lookup path.

## Why

Lookup of a key sharing a prefix with an existing key could access
memory beyond the valid node range.

## Testing

Tested:
- Empty trie
- Single key
- Prefix keys
- Missing keys
- 1M sequential inserts

## Benchmark

Before: 0.31 s
After: 0.29 s

Keep pull requests focused. If you discover an unrelated issue while working, open a separate issue or pull request for it.

Commits

Write clear commit messages that describe the change.

Good:

fix trie lookup for prefix keys
add benchmark for hashmap collisions
improve custom allocator handling

Avoid messages such as:

update
fix
changes
stuff
final

Small, logical commits are preferred, although contributors do not need to split trivial changes unnecessarily.

Reporting Bugs

When reporting a bug, provide enough information to reproduce it.

Include, where applicable:

Operating system

Compiler

Compiler version

Compiler flags

dtypes commit/version

Minimal reproduction

Expected behavior

Actual behavior

Error output

Relevant benchmark or test results


A minimal reproducible example is especially useful for memory-safety bugs.

Security Issues

Do not publicly disclose sensitive security issues before they can be investigated.

If a vulnerability involves memory corruption, out-of-bounds access, use-after-free, double-free, or another potentially serious issue, provide a minimal reproduction and enough technical information to reproduce it safely.

Design Philosophy

Contributions should generally preserve the project's core goals:

Generic data structures

Minimal dependencies

Consistent APIs

Custom allocator support

Efficient implementations

Simple integration into C projects


Not every data structure needs to make the same memory/performance trade-offs. Explain significant trade-offs when introducing or substantially changing an implementation.

Code of Conduct

Please follow the project's CODE_OF_CONDUCT.md.

Contributors are expected to keep discussions technical, constructive, and respectful.

License

By contributing to dtypes, you agree that your contributions may be distributed under the project's Apache License 2.0.
