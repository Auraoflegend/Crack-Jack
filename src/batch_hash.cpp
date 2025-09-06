#include "../include/batch_hash.h"
#include "../include/hash_utils.h"

#include <immintrin.h>
#include <string>
#include <vector>
#include <memory>

// === Scalar Engine ===
class ScalarHashEngine : public HashEngine {
public:
    ScalarHashEngine(HashAlgorithm algo) : algo(algo) {}

    std::string hashString(const std::string& input) override {
        return computeHash(algo, input);
    }

    std::vector<std::string> hashBatch(const std::vector<std::string>& inputs) override {
        std::vector<std::string> results;
        results.reserve(inputs.size());
        for (const auto& input : inputs) {
            results.push_back(hashString(input));
        }
        return results;
    }

private:
    HashAlgorithm algo;
};

// === AVX2 Engine (fallback to scalar for now) ===
class AVX2HashEngine : public HashEngine {
public:
    AVX2HashEngine(HashAlgorithm algo) : algo(algo), fallback(algo) {}

    std::string hashString(const std::string& input) override {
        return fallback.hashString(input);
    }

    std::vector<std::string> hashBatch(const std::vector<std::string>& inputs) override {
        return fallback.hashBatch(inputs);
    }

private:
    HashAlgorithm algo;
    ScalarHashEngine fallback;
};

// === Factory Function ===
std::unique_ptr<HashEngine> createHashEngine(HashAlgorithm algo) {
#ifdef __AVX2__
    return std::make_unique<AVX2HashEngine>(algo);
#else
    return std::make_unique<ScalarHashEngine>(algo);
#endif
}
