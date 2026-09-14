# C++ Latency Lab

<p align="center">
  <b>Small C++ experiments. Real measurements. Systems-level reasoning.</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-Performance-00599C?style=flat-square" />
  <img src="https://img.shields.io/badge/Compiler-GCC-lightgrey?style=flat-square" />
  <img src="https://img.shields.io/badge/Status-Active-success?style=flat-square" />
</p>

---

## About

A compact benchmarking lab exploring how C++ design choices affect **latency, memory behaviour, cache locality, allocation, and concurrency**.

The aim is to move beyond:

```text
knowing C++ syntax
```

towards:

```text
understanding what the machine is actually doing
```

---

## 01 — Vector vs List

**Workload**

```text
1,000,000 integers
same values
same summation
same compiler
same optimisation
```

### Initial result

| Container     |     Iteration |
| ------------- | ------------: |
| `std::vector` |    **629 µs** |
| `std::list`   | **10,312 µs** |

```text
std::vector   █ 629 µs

std::list     ████████████████ 10,312 µs
```

> In this run, `std::list` took roughly **16× longer** to iterate.

This was the original single-run baseline, so it should not be treated as a stable performance ratio. Individual timings can move because of scheduler activity, CPU frequency changes and other system noise.

### Current measurement method

The benchmark now uses 5 warm-up traversals followed by 50 measured runs for each container. It alternates which container is timed first, reports min/median/mean/max, and prints a checksum so the measured work has an observable result.

Because the benchmark deliberately warms up first, the current experiment is mainly a steady-state traversal comparison rather than a cold-cache test.

---

## Why?

### `std::vector`

```text
[0][1][2][3][4][5][6][7]
```

Elements are stored contiguously.

This generally gives the CPU:

* better cache locality
* predictable memory access
* effective hardware prefetching

### `std::list`

```text
[0] ─────► [1] ─────► [2] ─────► [3]
```

Each node may live somewhere else in memory.

Traversal requires repeated pointer following — commonly called **pointer chasing**.

Both are:

```text
O(n)
```

Yet their real-world latency can be very different.

---

## 02 — Map vs Unordered Map

Experiment 02 measures **successful random lookup latency** for `std::map` and `std::unordered_map` as the data set grows from 1,000 to 1,000,000 elements.

The benchmark uses pre-generated lookup keys, warm-up passes, 20 measured runs, alternating measurement order, median nanoseconds per lookup, and an observable checksum.

```text
std::map
key -> tree comparison -> pointer -> comparison -> pointer -> value

std::unordered_map
key -> hash -> bucket -> value
```

The goal is to connect algorithmic complexity with the cost of pointer chasing, hashing and memory locality rather than simply state that one container is "faster".

See [`experiments/02-map-vs-unordered-map`](experiments/02-map-vs-unordered-map) for the benchmark and methodology.

---

## Experiment flow

```mermaid
flowchart LR
    A[Design] --> B[Compile]
    B --> C[Benchmark]
    C --> D[Measure]
    D --> E[Understand]
```

---

## Lab

```text
cpp-latency-lab
│
└── experiments
    │
    ├── 01-vector-vs-list
    ├── 02-map-vs-unordered-map
    ├── 03-sequential-vs-random
    ├── 04-cache-line-stride
    ├── 05-aos-vs-soa
    ├── 06-false-sharing
    ├── 07-branch-prediction
    ├── 08-simd-vectorization
    └── 09-thread-scaling
```

---

## Roadmap

| Experiment                    | Focus                         | Status        |
| ----------------------------- | ----------------------------- | ------------- |
| `01` Vector vs List           | Cache locality                | ● Implemented |
| `02` Map vs Unordered Map     | Lookup latency                | ● Implemented |
| `03` Sequential vs Random     | Memory locality               | ○ Planned     |
| `04` Cache-Line Stride        | Cache lines / prefetching      | ○ Planned     |
| `05` AoS vs SoA               | Data layout                    | ○ Planned     |
| `06` False Sharing            | Cache coherence                | ○ Planned     |
| `07` Branch Prediction        | Predictability / control flow  | ○ Planned     |
| `08` SIMD Vectorization       | Compiler / data parallelism    | ○ Planned     |
| `09` Thread Scaling           | Parallel scaling               | ○ Planned     |

---

## Build

Experiment 01:

```bash
g++ -O2 -Wall -Wextra -Wpedantic experiments/main.cpp -o latency
./latency
```

Experiment 02:

```bash
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic experiments/02-map-vs-unordered-map/main.cpp -o map_lookup
./map_lookup
```

---

## Next

```text
run Experiment 02 on the target machine
        ↓
record median lookup latency
        ↓
compare scaling across container sizes
        ↓
move to sequential vs random memory access
```

---

<p align="center">
  <b>Built to understand performance — not just measure it.</b>
</p>

<p align="center">
  <a href="https://github.com/musaddiq2021">@musaddiq2021</a>
</p>
