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
    ├── 03-copy-vs-reference
    ├── 04-allocation
    ├── 05-threading
    └── 06-compiler-optimisation
```

---

## Roadmap

| Experiment                 | Focus            | Status    |
| -------------------------- | ---------------- | --------- |
| `01` Vector vs List        | Cache locality   | ● Active  |
| `02` Map vs Unordered Map  | Lookup latency   | ○ Planned |
| `03` Copy vs Reference     | Data movement    | ○ Planned |
| `04` Allocation            | Memory overhead  | ○ Planned |
| `05` Threading             | Concurrency cost | ○ Planned |
| `06` Compiler Optimisation | Generated code   | ○ Planned |

---

## Next

```text
repeated measurements
        ↓
min / max / mean / median
        ↓
record benchmark environment
        ↓
compiler comparison
        ↓
assembly inspection
```

---

## Build

From the repository root:

```bash
g++ -O2 -Wall -Wextra -Wpedantic experiments/main.cpp -o latency
./latency
```

---

<p align="center">
  <b>Built to understand performance — not just measure it.</b>
</p>

<p align="center">
  <a href="https://github.com/musaddiq2021">@musaddiq2021</a>
</p>
