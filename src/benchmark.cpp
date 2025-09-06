#include "../include/hash_utils.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<uint64_t> benchTries(0);
std::atomic<bool> benchStop(false);

std::string generateCandidate(uint64_t index, const std::string& charset, int length) {
    std::string result(length, charset[0]);
    size_t charsetLen = charset.size();
    for (int i = length - 1; i >= 0; --i) {
        result[i] = charset[index % charsetLen];
        index /= charsetLen;
    }
    return result;
}

void benchThread(const std::string& algo, const std::string& charset, int length) {
    uint64_t i = 0;
    while (!benchStop) {
        std::string candidate = generateCandidate(i++, charset, length);
        hashString(candidate, algo);
        ++benchTries;
    }
}

void run_benchmark(int threads, const std::string& algo, const std::string& charset, int length) {
    std::cout << "[Benchmark] Algorithm: " << algo << "\n";
    std::cout << "[Benchmark] Threads: " << threads << " | Charset length: " << charset.size() << " | Length: " << length << "\n";

    benchTries = 0;
    benchStop = false;
    std::vector<std::thread> pool;

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < threads; ++i) {
        pool.emplace_back(benchThread, algo, charset, length);
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));  // Short duration benchmark
    benchStop = true;

    for (auto& th : pool) th.join();

    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    uint64_t total = benchTries.load();
    double speed = total / elapsed;

    std::cout << "[Benchmark] Hashes/sec: " << static_cast<uint64_t>(speed) << "\n";
}
