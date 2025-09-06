#pragma once
#include <string>

struct OptimizerFlags {
    bool zero_byte = false;
    bool early_skip = false;
    bool not_salted = false;
    bool not_iterated = false;
    bool single_hash = false;
    bool single_salt = false;
    bool brute_force = false;
    bool raw_hash = false;
};

void detectAndPrintOptimizers(
    OptimizerFlags &optim,
    const std::string &mode,
    const std::string &algo,
    size_t hashCount = 1,
    size_t saltCount = 0
);
