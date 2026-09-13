# C++ Latency Lab

Small C++ experiments for learning how data layout, memory access and concurrency affect real program performance.

The goal is not to collect impressive benchmark numbers. It is to form a hypothesis, measure one variable carefully, and understand why the result happened.

## 01 — Vector vs List

The first experiment compares traversal of 1,000,000 integers stored in `std::vector` and `std::list`.

Both traversals are O(n), but the containers have very different memory layouts. `std::vector` stores elements contiguously, while `std::list` follows pointers between separately allocated nodes. That makes this a useful first look at cache locality and pointer chasing.

### Historical baseline

The first version of the experiment recorded one run:

| Container | Traversal |
| --- | ---: |
| `std::vector` | 629 us |
| `std::list` | 10,312 us |

That single measurement suggested a large difference, but one sample is not enough to make a strong performance claim. Scheduler activity, CPU frequency changes and other system noise can move an individual timing substantially.

### Current method

The benchmark now:

- performs 5 warm-up traversals
- records 50 measured runs for each container
- alternates which container is measured first
- reports min, median, mean and max traversal time
- prints a checksum so the measured work has an observable result
- uses `std::chrono::steady_clock`

The warm-up means this is primarily a steady-state traversal test. It should not be presented as a cold-cache benchmark.

Results should be recorded from a real run on the machine being documented rather than copied from another environment.

## Planned experiments

| # | Experiment | Main question |
| --- | --- | --- |
| 01 | Vector vs List | How does contiguous storage affect traversal? |
| 02 | Map vs Unordered Map | How do tree traversal and hashing affect lookup latency? |
| 03 | Sequential vs Random Access | How much does memory locality matter? |
| 04 | Cache-Line Stride | What happens as memory access becomes more sparse? |
| 05 | AoS vs SoA | How does data layout affect useful cache traffic? |
| 06 | False Sharing | How can independent threads interfere through cache coherence? |
| 07 | Branch Prediction | When do unpredictable branches become expensive? |
| 08 | SIMD / Vectorisation | When can the compiler process multiple values per instruction? |
| 09 | Thread Scaling | When does adding threads stop improving throughput? |

The experiments will be added progressively. A smaller set of well-understood benchmarks is more useful than many shallow examples.

## Build

From the repository root:

```bash
g++ -O2 -Wall -Wextra -Wpedantic experiments/main.cpp -o latency
./latency
```

The exact compiler version, optimisation flags and machine used for published results should be recorded alongside those results.

## Next

Run Experiment 01 repeatedly on the target machine and record a reproducible baseline. Once the methodology and results are stable, move to Experiment 02 rather than adding several experiments at once.
