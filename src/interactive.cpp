#include "../include/interactive.h"
#include "../include/brute_force_cracker.h"
#include "../include/wordlist_cracker.h"
#include "../include/hash_utils.h"
#include "../include/optimizer.h"
#include "../include/mask_util.h"

#include <iostream>
#include <fstream>

void launchInteractiveMode() {
    std::cout << "=== CrackJack Interactive Mode ===\n";
    std::cout << "[1] Brute-force attack\n";
    std::cout << "[2] Wordlist attack\n";
    std::cout << "[3] Rainbow table match (not implemented)\n";
    std::cout << "[4] PDF crack (not implemented)\n";
    std::cout << "[5] ZIP crack (not implemented)\n";
    std::cout << "> Choose an option: ";

    int choice;
    std::cin >> choice;

    std::string hash, algo, charset, wordlist, logPath;
    int minLen = 1, maxLen = 6, threads = 4;
    OptimizerFlags optim;

    switch (choice) {
        case 1:
            std::cout << "[+] Enter hash: ";
            std::cin >> hash;
            std::cout << "[+] Algorithm (md5/sha1/sha256/...): ";
            std::cin >> algo;
            std::cout << "[+] Charset (?l, ?a, etc.): ";
            std::cin >> charset;
            std::cout << "[+] Min length: ";
            std::cin >> minLen;
            std::cout << "[+] Max length: ";
            std::cin >> maxLen;
            std::cout << "[+] Threads: ";
            std::cin >> threads;
            std::cout << "[+] Optional log file (blank to skip): ";
            std::cin.ignore(); std::getline(std::cin, logPath);

            if (charset.find("?") != std::string::npos)
                charset = expandMaskCharset(charset);

            detectAndPrintOptimizers(optim, "brute", algo);
            crackBruteForceMultithreaded(hash, algo, charset, minLen, maxLen, threads,
                optim, logPath.empty() ? nullptr : new std::ofstream(logPath, std::ios::app));
            break;

        case 2:
            std::cout << "[+] Enter hash: ";
            std::cin >> hash;
            std::cout << "[+] Algorithm (md5/sha1/sha256/...): ";
            std::cin >> algo;
            std::cout << "[+] Wordlist file path: ";
            std::cin >> wordlist;
            std::cout << "[+] Optional log file (blank to skip): ";
            std::cin.ignore(); std::getline(std::cin, logPath);

            detectAndPrintOptimizers(optim, "wordlist", algo);
            crackWithWordlist(hash, algo, wordlist, logPath.empty() ? nullptr : new std::ofstream(logPath, std::ios::app));
            break;

        default:
            std::cout << "[!] This mode is not implemented yet.\n";
            break;
    }
}
