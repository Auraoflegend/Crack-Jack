#include "../include/brute_force_cracker.h"
#include "../include/hash_utils.h"
#include "../include/mask_util.h"
#include "../include/optimizer.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <cmath>
#include <windows.h>
#include <iomanip>

std::atomic<bool> found(false);
std::atomic<uint64_t> totalTries(0);
std::mutex outputMutex;
std::chrono::steady_clock::time_point globalStart;
uint64_t globalKeyspace = 0; // ✅ Added global keyspace tracker

// ====== Utilities ======
uint64_t pow64(uint64_t base, uint64_t exp) {
    uint64_t result = 1;
    while (exp--) result *= base;
    return result;
}

std::string indexToCandidate(uint64_t index, const std::string& charset, int length) {
    std::string result(length, charset[0]);
    size_t charsetLen = charset.size();
    for (int i = length - 1; i >= 0; --i) {
        result[i] = charset[index % charsetLen];
        index /= charsetLen;
    }
    return result;
}

// ====== Status Output with Global Keyspace ======
void showStatus(const std::string& last, double elapsed, uint64_t tries, uint64_t totalKeyspace) {
    double speed = tries / elapsed;
    uint64_t remaining = totalKeyspace - tries;

    // Avoid division by zero
    uint64_t etaSec = speed > 0 ? static_cast<uint64_t>(remaining / speed) : 0;

    int days = etaSec / 86400;
    int hours = (etaSec % 86400) / 3600;
    int minutes = (etaSec % 3600) / 60;
    int seconds = etaSec % 60;

    std::lock_guard<std::mutex> lock(outputMutex);
    std::cout << "[Status] Tries: " << tries
              << " | Last: " << last
              << " | Speed: " << static_cast<uint64_t>(speed) << "/sec"
              << " | ETA: ";

    if (days > 0)
        std::cout << days << "d ";

    std::cout << std::setw(2) << std::setfill('0') << hours << ":"
              << std::setw(2) << std::setfill('0') << minutes << ":"
              << std::setw(2) << std::setfill('0') << seconds
              << std::endl;
}

// ====== Worker Function ======
void crackRange(const std::string& targetHash, const std::string& algorithm,
                const std::string& charset, int length,
                uint64_t start, uint64_t end,
                const OptimizerFlags& optim) {
    std::string candidate;

    for (uint64_t i = start; i < end && !found; ++i) {
        candidate = indexToCandidate(i, charset, length);
        ++totalTries;

        if (optim.early_skip && targetHash.size() >= 4) {
            std::string prefix = targetHash.substr(0, 4);
            std::string hashPrefix = hashString(candidate, algorithm).substr(0, 4);
            if (prefix != hashPrefix) continue;
        }

        std::string hashed = hashString(candidate, algorithm);

        if (hashed == targetHash) {
            found = true;
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << targetHash << ":" << algorithm << ":" << candidate << std::endl;
            return;
        }

        // Show status on spacebar
        if ((totalTries % 5000 == 0) && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            double elapsed = std::chrono::duration<double>(
                std::chrono::steady_clock::now() - globalStart).count();
            showStatus(candidate, elapsed, totalTries, globalKeyspace);  // ✅ Use globalKeyspace here
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
}

// ====== Brute Force Engine ======
bool crackBruteForceMultithreaded(const std::string& targetHash,
                                  const std::string& algorithm,
                                  const std::string& mask,
                                  int minLen, int maxLen,
                                  int numThreads,
                                  const OptimizerFlags& optim) {
    std::string charset = expandMaskCharset(mask);
    totalTries = 0;
    found = false;
    globalStart = std::chrono::steady_clock::now();

    for (int len = minLen; len <= maxLen; ++len) {
        globalKeyspace = pow64(charset.size(), len);  // ✅ Track total keyspace for ETA
        uint64_t totalComb = globalKeyspace;

        uint64_t perThread = totalComb / numThreads;
        uint64_t remainder = totalComb % numThreads;

        std::vector<std::thread> threads;
        uint64_t start = 0;

        for (int t = 0; t < numThreads; ++t) {
            uint64_t end = start + perThread + (t < remainder ? 1 : 0);
            threads.emplace_back(crackRange, targetHash, algorithm, charset, len, start, end, optim);
            start = end;
        }

        for (auto& th : threads) th.join();

        if (found) return true;
    }

    if (!found) {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << targetHash << ":" << algorithm << ":NOT_FOUND" << std::endl;
    }

    double totalTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - globalStart).count();
    double speed = totalTries / totalTime;

    std::cout << "[Done] Time: " << totalTime << "s"
              << " | Attempts: " << totalTries
              << " | Speed: " << static_cast<uint64_t>(speed) << "/sec" << std::endl;

    return found;
}
