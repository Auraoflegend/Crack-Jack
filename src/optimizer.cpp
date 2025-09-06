#include "../include/optimizer.h"
#include <iostream>

void detectAndPrintOptimizers(
    OptimizerFlags &optim,
    const std::string &mode,
    const std::string &algo,
    size_t hashCount,
    size_t saltCount
) {
    std::cout << "\nCrackJack (v1.0.0) starting\n\n";

    if (mode == "brute")          optim.brute_force = true;
    if (hashCount == 1)           optim.single_hash = true;
    if (saltCount <= 1)           optim.single_salt = true;

    // Assume most common algorithms are raw, not salted, and not iterated
    if (algo == "md5" || algo == "sha1" || algo == "sha256" ||
        algo == "sha512" || algo == "ntlm") {
        optim.raw_hash = true;
        optim.not_salted = true;
        optim.not_iterated = true;
    }

    std::cout << "Optimizers applied:\n";
    if (optim.zero_byte)     std::cout << "* Zero-Byte\n";
    if (optim.early_skip)    std::cout << "* Early-Skip\n";
    if (optim.not_salted)    std::cout << "* Not-Salted\n";
    if (optim.not_iterated)  std::cout << "* Not-Iterated\n";
    if (optim.single_hash)   std::cout << "* Single-Hash\n";
    if (optim.single_salt)   std::cout << "* Single-Salt\n";
    if (optim.brute_force)   std::cout << "* Brute-Force\n";
    if (optim.raw_hash)      std::cout << "* Raw-Hash\n";
    std::cout << std::endl;
}
