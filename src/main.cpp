#include "../include/hash_utils.h"
#include "../include/optimizer.h"
#include "../include/wordlist_cracker.h"
#include "../include/brute_force_cracker.h"
#include "../include/mask_util.h"
#include "../include/benchmark.h"
#include "../include/interactive.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstring>

// === Usage Print ===
void printUsage() {
    std::cout << "Usage:\n"
              << "  crackjack --mode hash --input STRING|FILE\n"
              << "  crackjack --mode crack --algo ALGO --hash HASH|--hashes FILE --wordlist FILE [--log output.txt]\n"
              << "  crackjack --mode crack --algo ALGO --hash HASH --brute --charset MASK --min LEN --max LEN --threads N [--log output.txt] [--resume checkpoint.txt]\n"
              << "  crackjack --benchmark --algo ALGO --threads N --charset MASK --length LEN\n"
              << "  crackjack --interactive\n";
}

// === CLI Argument Parser ===
std::unordered_map<std::string, std::string> parseArgs(int argc, char *argv[]) {
    std::unordered_map<std::string, std::string> args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            std::string value = (i + 1 < argc && std::string(argv[i + 1]).substr(0, 2) != "--")
                                ? argv[++i] : "";
            args[key] = value;
        }
    }

    // Explicit boolean flags
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--brute") args["brute"] = "true";
        if (flag == "--benchmark") args["benchmark"] = "true";
        if (flag == "--interactive") args["interactive"] = "true";
    }

    return args;
}

// === MAIN ===
int main(int argc, char *argv[]) {
    auto args = parseArgs(argc, argv);
    OptimizerFlags optim;

    // === Interactive Mode ===
    if (args.count("interactive")) {
        launchInteractiveMode();
        return 0;
    }

    // === Benchmark Mode ===
    if (args.count("benchmark")) {
        std::string algo = args.count("algo") ? args["algo"] : "md5";
        int threads = args.count("threads") ? std::stoi(args["threads"]) : 4;
        std::string charset = args.count("charset") ? expandMaskCharset(args["charset"]) : "abcdefghijklmnopqrstuvwxyz";
        int length = args.count("length") ? std::stoi(args["length"]) : 6;

        run_benchmark(threads, algo, charset, length);
        return 0;
    }

    // === Hash Cracking Mode ===
    if (args["mode"] == "crack") {
        std::string hash = args["hash"];
        std::string algorithm = args["algo"];
        detectAndPrintOptimizers(optim, "brute", algorithm);

        std::ofstream logFile;
        if (args.count("log")) {
            logFile.open(args["log"], std::ios::app);
            if (!logFile.is_open()) {
                std::cerr << "[!] Failed to open log file: " << args["log"] << "\n";
                return 1;
            }
        }

        if (args.count("wordlist")) {
            std::string wordlist = args["wordlist"];
            crackWithWordlist(hash, algorithm, wordlist, logFile.is_open() ? &logFile : nullptr);
        }
        else if (args.count("brute")) {
            std::string charset = args["charset"];
            int minLen = std::stoi(args["min"]);
            int maxLen = std::stoi(args["max"]);
            int threads = std::stoi(args["threads"]);
            std::string resumePath = args.count("resume") ? args["resume"] : "";

            if (charset.find("?") != std::string::npos) {
                charset = expandMaskCharset(charset);
            }

            crackBruteForceMultithreaded(hash, algorithm, charset, minLen, maxLen, threads, optim,
                                         logFile.is_open() ? &logFile : nullptr, resumePath);
        } else {
            std::cerr << "[!] Invalid crack mode: must specify --wordlist or --brute\n";
            printUsage();
        }

        if (logFile.is_open()) logFile.close();
        return 0;
    }

    // === Hashing Mode ===
    if (args["mode"] == "hash") {
        std::ifstream infile(args["input"]);
        if (infile.good()) {
            std::string line;
            while (std::getline(infile, line)) {
                std::cout << "MD5    : " << md5(line) << "\n";
                std::cout << "SHA1   : " << sha1(line) << "\n";
                std::cout << "SHA256 : " << sha256(line) << "\n";
                std::cout << "SHA512 : " << sha512(line) << "\n";
                std::cout << "NTLM   : " << ntlm(line) << "\n\n";
            }
        } else {
            std::string input = args["input"];
            std::cout << "MD5    : " << md5(input) << "\n";
            std::cout << "SHA1   : " << sha1(input) << "\n";
            std::cout << "SHA256 : " << sha256(input) << "\n";
            std::cout << "SHA512 : " << sha512(input) << "\n";
            std::cout << "NTLM   : " << ntlm(input) << "\n";
        }

        return 0;
    }

    // === Fallback ===
    printUsage();
    return 1;
}
