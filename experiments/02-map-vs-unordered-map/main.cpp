#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::nanoseconds;

constexpr std::size_t kWarmupRuns = 3;
constexpr std::size_t kMeasuredRuns = 20;
constexpr std::size_t kLookupsPerRun = 250'000;
constexpr std::uint32_t kSeed = 42;

struct Measurement {
    double ns_per_lookup{};
    std::uint64_t checksum{};
};

template <typename Container>
Measurement measure_lookup(const Container& container,
                           const std::vector<int>& lookup_keys) {
    std::uint64_t checksum = 0;

    const auto start = Clock::now();

    for (const int key : lookup_keys) {
        const auto it = container.find(key);
        if (it != container.end()) {
            checksum += static_cast<std::uint64_t>(it->second);
        }
    }

    const auto end = Clock::now();
    const auto elapsed_ns =
        std::chrono::duration_cast<Nanoseconds>(end - start).count();

    return {
        static_cast<double>(elapsed_ns) /
            static_cast<double>(lookup_keys.size()),
        checksum,
    };
}

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    const std::size_t middle = values.size() / 2;

    if (values.size() % 2 == 0) {
        return (values[middle - 1] + values[middle]) / 2.0;
    }

    return values[middle];
}

std::vector<int> make_lookup_keys(std::size_t element_count) {
    std::mt19937 rng(kSeed);
    std::uniform_int_distribution<int> distribution(
        0, static_cast<int>(element_count - 1));

    std::vector<int> keys;
    keys.reserve(kLookupsPerRun);

    for (std::size_t i = 0; i < kLookupsPerRun; ++i) {
        keys.push_back(distribution(rng));
    }

    return keys;
}

void run_case(std::size_t element_count, std::uint64_t& checksum_sink) {
    std::map<int, int> ordered;
    std::unordered_map<int, int> hashed;
    hashed.reserve(element_count);

    for (std::size_t i = 0; i < element_count; ++i) {
        const int key = static_cast<int>(i);
        const int value = key * 2;

        ordered.emplace(key, value);
        hashed.emplace(key, value);
    }

    const auto lookup_keys = make_lookup_keys(element_count);

    for (std::size_t run = 0; run < kWarmupRuns; ++run) {
        checksum_sink += measure_lookup(ordered, lookup_keys).checksum;
        checksum_sink += measure_lookup(hashed, lookup_keys).checksum;
    }

    std::vector<double> map_samples;
    std::vector<double> unordered_samples;
    map_samples.reserve(kMeasuredRuns);
    unordered_samples.reserve(kMeasuredRuns);

    for (std::size_t run = 0; run < kMeasuredRuns; ++run) {
        if (run % 2 == 0) {
            const auto map_result = measure_lookup(ordered, lookup_keys);
            const auto unordered_result = measure_lookup(hashed, lookup_keys);

            map_samples.push_back(map_result.ns_per_lookup);
            unordered_samples.push_back(unordered_result.ns_per_lookup);
            checksum_sink += map_result.checksum + unordered_result.checksum;
        } else {
            const auto unordered_result = measure_lookup(hashed, lookup_keys);
            const auto map_result = measure_lookup(ordered, lookup_keys);

            unordered_samples.push_back(unordered_result.ns_per_lookup);
            map_samples.push_back(map_result.ns_per_lookup);
            checksum_sink += map_result.checksum + unordered_result.checksum;
        }
    }

    const double map_median = median(map_samples);
    const double unordered_median = median(unordered_samples);
    const double speedup = map_median / unordered_median;

    std::cout << std::left << std::setw(14) << element_count
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(18) << map_median
              << std::setw(24) << unordered_median
              << std::setw(14) << speedup << "x\n";
}

}  // namespace

int main() {
    const std::vector<std::size_t> sizes{
        1'000,
        10'000,
        100'000,
        1'000'000,
    };

    std::uint64_t checksum_sink = 0;

    std::cout << "Experiment 02: std::map vs std::unordered_map\n"
              << "Successful random lookups, median of " << kMeasuredRuns
              << " measured runs after " << kWarmupRuns << " warm-up runs\n"
              << "Lookups per run: " << kLookupsPerRun << "\n\n";

    std::cout << std::left << std::setw(14) << "Elements"
              << std::right << std::setw(18) << "map ns/lookup"
              << std::setw(24) << "unordered ns/lookup"
              << std::setw(14) << "Speedup" << '\n';
    std::cout << std::string(70, '-') << '\n';

    for (const std::size_t size : sizes) {
        run_case(size, checksum_sink);
    }

    std::cout << "\nChecksum: " << checksum_sink << '\n';
    return 0;
}
