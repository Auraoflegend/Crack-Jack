#include "../include/brute_force_cracker.h"
#include "../include/hash_utils.h"
#include "../include/mask_util.h"
#include "../include/optimizer.h"
#include "../include/resume_manager.h"
#include "../include/batch_hash.h"

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
#include <fstream>
#include <cstdio>
#include <algorithm> // for std::transform

std::atomic<bool> found(false);
std::atomic<uint64_t> totalTries(0);
std::atomic<uint64_t> globalIndex(0);

std::mutex outputMutex;
std::chrono::steady_clock::time_point globalStart;
uint64_t globalKeyspace = 0;
constexpr uint64_t BLOCK_SIZE = 10000;

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

// ✅ Add this helper inline function if not already in hash_utils.h
inline HashAlgorithm stringToAlgo(const std::string& name) {
    std::string algo = name;
    std::transform(algo.begin(), algo.end(), algo.begin(), ::tolower);

    if (algo == "md5") return HashAlgorithm::MD5;
    if (algo == "sha1") return HashAlgorithm::SHA1;
    if (algo == "sha256") return HashAlgorithm::SHA256;
    if (algo == "sha512") return HashAlgorithm::SHA512;
    if (algo == "ntlm") return HashAlgorithm::NTLM;

    throw std::runtime_error("Unsupported algorithm: " + name);
}

void showStatus(const std::string& last, double elapsed, uint64_t tries, uint64_t totalKeyspace) {
    double speed = tries / elapsed;
    uint64_t remaining = totalKeyspace - tries;
    uint64_t etaSec = speed > 0 ? static_cast<uint64_t>(remaining / speed) : 0;

    int years = etaSec / (365 * 24 * 3600);
    etaSec %= (365 * 24 * 3600);
    int months = etaSec / (30 * 24 * 3600);
    etaSec %= (30 * 24 * 3600);
    int days = etaSec / 86400;
    etaSec %= 86400;
    int hours = etaSec / 3600;
    etaSec %= 3600;
    int minutes = etaSec / 60;
    int seconds = etaSec % 60;

    std::lock_guard<std::mutex> lock(outputMutex);
    std::cout << "[Status] Tries: " << tries
              << " | Last: " << last
              << " | Speed: " << static_cast<uint64_t>(speed) << "/sec"
              << " | ETA: ";

    if (years > 0)   std::cout << years << "y ";
    if (months > 0)  std::cout << months << "m ";
    if (days > 0)    std::cout << days << "d ";

    std::cout << std::setw(2) << std::setfill('0') << hours << ":"
              << std::setw(2) << std::setfill('0') << minutes << ":"
              << std::setw(2) << std::setfill('0') << seconds
              << std::endl;
}

void crackRangeDispatcher(const std::string& targetHash, const std::string& algorithm,
                          const std::string& charset, int length,
                          const OptimizerFlags& optim,
                          std::string* crackedPassword,
                          const std::string& resumePath,
                          int threadID, uint64_t totalKeyspace) {
    auto engine = createHashEngine(stringToAlgo(algorithm));
    std::string lastCandidate;
    std::vector<std::string> batch;
    batch.reserve(BLOCK_SIZE);

    while (!found) {
        uint64_t start = globalIndex.fetch_add(BLOCK_SIZE);
        if (start >= totalKeyspace) break;
        uint64_t end = std::min(start + BLOCK_SIZE, totalKeyspace);

        batch.clear();
        for (uint64_t i = start; i < end && !found; ++i) {
            std::string candidate = indexToCandidate(i, charset, length);

            if (optim.early_skip && targetHash.size() >= 4) {
                std::string prefix = targetHash.substr(0, 4);
                std::string hashPrefix = hashString(candidate, algorithm).substr(0, 4);
                if (prefix != hashPrefix) continue;
            }

            batch.push_back(candidate);
        }

        if (batch.empty()) continue;

        std::vector<std::string> results = engine->hashBatch(batch);

        for (size_t i = 0; i < results.size(); ++i) {
            ++totalTries;
            lastCandidate = batch[i];

            if (results[i] == targetHash) {
                found = true;
                *crackedPassword = batch[i];
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << targetHash << ":" << algorithm << ":" << batch[i] << std::endl;
                return;
            }

            if ((totalTries % 5000 == 0) && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
                double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - globalStart).count();
                showStatus(lastCandidate, elapsed, totalTries, totalKeyspace);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }

            if (threadID == 0 && !resumePath.empty() && totalTries % 100000 == 0) {
                ResumeState state(length, globalIndex.load());
                state.save(resumePath);
            }
        }
    }
}

bool crackBruteForceMultithreaded(const std::string& targetHash,
                                  const std::string& algorithm,
                                  const std::string& mask,
                                  int minLen, int maxLen,
                                  int numThreads,
                                  const OptimizerFlags& optim,
                                  std::ostream* logStream,
                                  const std::string& resumePath) {
    std::string charset = expandMaskCharset(mask);
    totalTries = 0;
    found = false;
    globalStart = std::chrono::steady_clock::now();
    std::string crackedPassword = "";

    ResumeState resume;
    bool resumeLoaded = false;
    if (!resumePath.empty()) {
        resumeLoaded = resume.load(resumePath);
        if (resumeLoaded)
            std::cout << "[Resume] Loaded checkpoint: Length=" << resume.length << ", Index=" << resume.index << "\n";
    }

    for (int len = minLen; len <= maxLen; ++len) {
        if (resumeLoaded && len < resume.length) continue;

        globalKeyspace = pow64(charset.size(), len);
        globalIndex = (resumeLoaded && len == resume.length) ? resume.index : 0;

        std::vector<std::thread> threads;
        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back(crackRangeDispatcher, targetHash, algorithm, charset, len,
                                 optim, &crackedPassword, resumePath, t, globalKeyspace);
        }

        for (auto& th : threads) th.join();

        if (found) {
            if (!resumePath.empty()) {
                std::remove(resumePath.c_str());
            }

            double totalTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - globalStart).count();
            double speed = totalTries / totalTime;

            if (logStream) {
                (*logStream) << "[LOG] Cracked: true\n";
                (*logStream) << "Hash   : " << targetHash << "\n";
                (*logStream) << "Algo   : " << algorithm << "\n";
                (*logStream) << "Tries  : " << totalTries << "\n";
                (*logStream) << "Time   : " << totalTime << "s\n";
                (*logStream) << "Speed  : " << static_cast<uint64_t>(speed) << "/sec\n";
                (*logStream) << "Password: " << crackedPassword << "\n";
                (*logStream) << "-----------------------------\n";
            }

            return true;
        }
    }

    double totalTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - globalStart).count();
    double speed = totalTries / totalTime;

    std::lock_guard<std::mutex> lock(outputMutex);
    std::cout << targetHash << ":" << algorithm << ":NOT_FOUND" << std::endl;

    if (logStream) {
        (*logStream) << "[LOG] Cracked: false\n";
        (*logStream) << "Hash   : " << targetHash << "\n";
        (*logStream) << "Algo   : " << algorithm << "\n";
        (*logStream) << "Tries  : " << totalTries << "\n";
        (*logStream) << "Time   : " << totalTime << "s\n";
        (*logStream) << "Speed  : " << static_cast<uint64_t>(speed) << "/sec\n";
        (*logStream) << "Password: NOT_FOUND\n";
        (*logStream) << "-----------------------------\n";
    }

    std::cout << "[Done] Time: " << totalTime << "s"
              << " | Attempts: " << totalTries
              << " | Speed: " << static_cast<uint64_t>(speed) << "/sec" << std::endl;

    return false;
}
