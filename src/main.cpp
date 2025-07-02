#include "../include/hash_utils.h"
#include "../include/optimizer.h"
#include "../include/wordlist_cracker.h"
#include "../include/brute_force_cracker.h"
#include "../include/mask_util.h"

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cstring>




// === CLI Argument Parser ===
std::unordered_map<std::string, std::string> parseArgs(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc - 1; ++i) {
        std::string key = argv[i];
        if (key.substr(0, 2) == "--") {
            std::string value = argv[i + 1];
            if (value.substr(0, 2) != "--") {
                args[key.substr(2)] = value;
                ++i;
            } else {
                args[key.substr(2)] = "";
            }
        }
    }

    // Handle flags like --brute with no value
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--brute") args["brute"] = "true";
    }

    return args;
}

// === Usage Print ===
void printUsage() {
    std::cout << "Usage:\n"
              << "  crackjack --mode hash --input STRING|FILE\n"
              << "  crackjack --mode crack --algo ALGO --hash HASH|--hashes FILE "
              << "--wordlist FILE\n"
              << "  crackjack --mode crack --algo ALGO --hash HASH "
              << "--brute --charset MASK --min LEN --max LEN --threads N\n";
}

// === Optimizer Banner Print ===
void detectAndPrintOptimizers(OptimizerFlags& optim, const std::string& mode, const std::string& algo, size_t hashCount = 1, size_t saltCount = 0) {
    std::cout << "\nCrackJack (v1.0.0) starting\n\n";

    if (mode == "brute")          optim.brute_force = true;
    if (hashCount == 1)           optim.single_hash = true;
    if (saltCount <= 1)           optim.single_salt = true;

    HashType type = HASH_UNKNOWN;
    if (algo == "md5")      type = HASH_MD5;
    else if (algo == "sha1")    type = HASH_SHA1;
    else if (algo == "sha256")  type = HASH_SHA256;
    else if (algo == "sha512")  type = HASH_SHA512;
    else if (algo == "ntlm")    type = HASH_NTLM;
    else if (algo == "bcrypt")  type = HASH_BCRYPT;
    else if (algo == "pbkdf2")  type = HASH_PBKDF2;
    else if (algo == "scrypt")  type = HASH_SCRYPT;

    if (!isSalted(type))        optim.not_salted = true;
    if (!isIterated(type))      optim.not_iterated = true;
    if (isRawHash(type))        optim.raw_hash = true;

    // Print detected optimizers
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

// === MAIN ===
int main(int argc, char* argv[]) {
    auto args = parseArgs(argc, argv);
    OptimizerFlags optim;

    if (args["mode"] == "hash") {
        std::ifstream infile(args["input"]);
        if (infile.good()) {
            std::string line;
            while (std::getline(infile, line)) {
                std::cout << "MD5    : " << md5(line) << "\n";
                std::cout << "SHA1   : " << sha1(line) << "\n";
                std::cout << "SHA256 : " << sha256(line) << "\n";
                std::cout << "SHA512 : " << sha512(line) << "\n";
                std::cout << "NTLM   : " << ntlm(line) << "\n";
                std::cout << std::endl;
            }
        } else {
            std::string input = args["input"];
            std::cout << "MD5    : " << md5(input) << "\n";
            std::cout << "SHA1   : " << sha1(input) << "\n";
            std::cout << "SHA256 : " << sha256(input) << "\n";
            std::cout << "SHA512 : " << sha512(input) << "\n";
            std::cout << "NTLM   : " << ntlm(input) << "\n";
        }
    }

    else if (args["mode"] == "crack") {
        std::string hash = args["hash"];
        std::string algorithm = args["algo"];

        detectAndPrintOptimizers(optim, "brute", algorithm);

        if (args.count("wordlist")) {
            std::string wordlist = args["wordlist"];
            crackWithWordlist(hash, algorithm, wordlist);
        }

        else if (args.count("brute")) {
            std::string charset = args["charset"];
            int minLen = std::stoi(args["min"]);
            int maxLen = std::stoi(args["max"]);
            int threads = std::stoi(args["threads"]);

            if (charset.find("?") != std::string::npos) {
                charset = expandMaskCharset(charset);
            }

            crackBruteForceMultithreaded(hash, algorithm, charset, minLen, maxLen, threads, optim);
        }

        else {
            std::cerr << "[!] Invalid crack mode: must specify --wordlist or --brute\n";
            printUsage();
        }
    }

    else {
        printUsage();
    }

    return 0;
}
