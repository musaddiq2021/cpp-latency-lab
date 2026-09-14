#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>

using Clock = std::chrono::steady_clock;

struct TimingResult {
    std::int64_t nanoseconds;
    long long sum;
};

template <typename Container>
TimingResult time_traversal(const Container& values)
{
    const auto start = Clock::now();

    long long sum = 0;
    for (int value : values) {
        sum += value;
    }

    const auto end = Clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    return {elapsed.count(), sum};
}

struct Statistics {
    double min_us;
    double median_us;
    double mean_us;
    double max_us;
};

Statistics calculate_statistics(std::vector<std::int64_t> samples)
{
    std::sort(samples.begin(), samples.end());

    const auto total = std::accumulate(
        samples.begin(), samples.end(), std::int64_t{0});
    const std::size_t middle = samples.size() / 2;

    double median_ns = static_cast<double>(samples[middle]);
    if (samples.size() % 2 == 0) {
        median_ns = (static_cast<double>(samples[middle - 1]) +
                     static_cast<double>(samples[middle])) /
                    2.0;
    }

    return {
        samples.front() / 1000.0,
        median_ns / 1000.0,
        (static_cast<double>(total) / samples.size()) / 1000.0,
        samples.back() / 1000.0,
    };
}

void print_statistics(const char* name, const Statistics& stats)
{
    std::cout << std::left << std::setw(10) << name
              << " min " << std::setw(9) << stats.min_us
              << " median " << std::setw(9) << stats.median_us
              << " mean " << std::setw(9) << stats.mean_us
              << " max " << stats.max_us << " us\n";
}

int main()
{
    constexpr int size = 1'000'000;
    constexpr int warmup_runs = 5;
    constexpr int measured_runs = 50;

    std::vector<int> numbers;
    numbers.reserve(size);
    std::list<int> number_list;

    for (int i = 0; i < size; ++i) {
        numbers.push_back(i);
        number_list.push_back(i);
    }

    long long checksum = 0;

    for (int i = 0; i < warmup_runs; ++i) {
        checksum += time_traversal(numbers).sum;
        checksum += time_traversal(number_list).sum;
    }

    std::vector<std::int64_t> vector_samples;
    std::vector<std::int64_t> list_samples;
    vector_samples.reserve(measured_runs);
    list_samples.reserve(measured_runs);

    for (int run = 0; run < measured_runs; ++run) {
        if (run % 2 == 0) {
            const auto vector_result = time_traversal(numbers);
            const auto list_result = time_traversal(number_list);
            vector_samples.push_back(vector_result.nanoseconds);
            list_samples.push_back(list_result.nanoseconds);
            checksum += vector_result.sum + list_result.sum;
        } else {
            const auto list_result = time_traversal(number_list);
            const auto vector_result = time_traversal(numbers);
            list_samples.push_back(list_result.nanoseconds);
            vector_samples.push_back(vector_result.nanoseconds);
            checksum += vector_result.sum + list_result.sum;
        }
    }

    const auto vector_stats = calculate_statistics(vector_samples);
    const auto list_stats = calculate_statistics(list_samples);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Traversal benchmark: " << size << " integers, "
              << measured_runs << " measured runs\n";
    std::cout << "Warm-up runs: " << warmup_runs << "\n\n";

    print_statistics("vector", vector_stats);
    print_statistics("list", list_stats);

    std::cout << "\nMedian ratio (list/vector): "
              << list_stats.median_us / vector_stats.median_us << "x\n";
    std::cout << "Checksum: " << checksum << '\n';

    return 0;
}
