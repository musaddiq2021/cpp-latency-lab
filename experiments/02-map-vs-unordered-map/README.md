# Experiment 02 — `std::map` vs `std::unordered_map`

## Question

How does lookup latency change between `std::map` and `std::unordered_map` as the number of stored elements grows?

## Hypothesis

For successful random lookups, `std::unordered_map` should usually be faster on average because hashing provides average-case constant-time lookup, while `std::map` must traverse a balanced tree in logarithmic time.

The interesting part is not only the Big-O difference. `std::map` also follows pointers through separately allocated tree nodes, which can increase cache misses and memory-access latency as the structure grows.

## Method

The benchmark uses the same integer key/value pairs in both containers and measures successful random lookups at four sizes:

- 1,000 elements
- 10,000 elements
- 100,000 elements
- 1,000,000 elements

For each size:

- 250,000 lookup keys are generated before timing begins.
- Both containers receive exactly the same keys and values.
- `std::unordered_map::reserve()` is called before insertion so construction rehashing does not affect the final benchmark setup.
- 3 warm-up runs are performed.
- 20 measured runs are collected.
- The order of the two containers is alternated between measured runs to reduce first/second ordering bias.
- Median latency is reported in nanoseconds per lookup.
- A checksum is printed so the lookup work produces an observable result and cannot simply be discarded by the optimiser.

The benchmark is intentionally focused on steady-state successful lookup latency. Insertion cost, failed lookups, deliberately poor hash functions and memory usage are separate questions.

## Build

From the repository root:

```bash
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic experiments/02-map-vs-unordered-map/main.cpp -o map_lookup
./map_lookup
```

On Windows/MSYS2 the executable may be named `map_lookup.exe`.

## Output

The program prints one row per container size:

```text
Elements           map ns/lookup     unordered ns/lookup       Speedup
----------------------------------------------------------------------
...
```

Actual benchmark numbers are intentionally not hard-coded here. They should come from a real run on the machine being documented.

## What is happening?

### `std::map`

`std::map` is normally implemented as a balanced tree. A lookup repeatedly compares the key and follows either a left or right child pointer until the target node is found.

Conceptually:

```text
root
  |
comparison
  |
next node
  |
comparison
  |
next node
```

That gives logarithmic lookup complexity, but the nodes are typically separate allocations. As the tree grows, pointer chasing can make cache locality worse.

### `std::unordered_map`

`std::unordered_map` hashes the key to select a bucket and then searches within that bucket.

Conceptually:

```text
key -> hash -> bucket -> value
```

With a reasonable hash function and load factor, lookup is average-case constant time. It is not guaranteed constant time: collisions can increase the work, and worst-case lookup remains linear.

## What I learned

Big-O complexity explains part of the result, but lookup latency also depends on how the data structure interacts with the memory hierarchy. A tree can require several dependent memory accesses before reaching a value, while a hash table can often reach the relevant bucket with fewer dependent steps.

The benchmark also shows why one timing is not enough. Repeated runs, warm-up, median reporting and alternating measurement order produce a more credible comparison than a single stopwatch reading.

## Limitations

Results are machine- and implementation-dependent. They can change with compiler version, standard-library implementation, CPU frequency, cache hierarchy, background processes, allocator behaviour and hash-table load factor.

This experiment measures only successful integer-key lookups using the standard hash function. It does not yet measure misses, insertion/erase latency, memory overhead, collision-heavy workloads or custom hash functions.
