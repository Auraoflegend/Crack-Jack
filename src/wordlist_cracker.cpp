#include "../include/wordlist_cracker.h"
#include "../include/hash_utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <windows.h>

void crackWithWordlist(const std::string& target, const std::string& algo, const std::string& wordlistPath, std::ostream* log) {
    std::ifstream file(wordlistPath);
    if (!file) {
        std::cerr << "[!] Could not open wordlist: " << wordlistPath << "\n";
        return;
    }

    std::vector<std::string> words;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) words.push_back(line);
    }

    size_t totalWords = words.size();
    size_t tries = 0;
    bool found = false;
    std::string crackedPassword = "";
    auto start = std::chrono::steady_clock::now();

    for (const auto& word : words) {
        std::string hashed = hashString(word, algo);
        ++tries;

        if (target == hashed) {
            std::string result = target + ":" + algo + ":" + word;
            std::cout << result << "\n";
            if (log) (*log) << result << "\n";
            found = true;
            crackedPassword = word;
            break;
        }

        // Optional: show live status on Spacebar
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - start).count();
            double speed = tries / elapsed;
            double eta = (speed > 0) ? (totalWords - tries) / speed : 0;

            std::cout << "[Status] Tries: " << tries
                      << " | Last: " << word
                      << " | Elapsed: " << elapsed << "s"
                      << " | ETA: " << eta << "s"
                      << " | Speed: " << static_cast<size_t>(speed) << "/sec"
                      << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    if (!found) {
        std::string result = target + ":" + algo + ":NOT_FOUND";
        std::cout << result << "\n";
        if (log) (*log) << result << "\n";
    }

    auto end = std::chrono::steady_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    double speed = tries / duration;

    std::cout << "[Done] Time: " << duration << "s"
              << " | Attempts: " << tries
              << " | Speed: " << static_cast<size_t>(speed) << "/sec"
              << std::endl;

    if (log) {
        (*log) << "[LOG] Cracked: " << (found ? "true" : "false") << "\n";
        (*log) << "Hash   : " << target << "\n";
        (*log) << "Algo   : " << algo << "\n";
        (*log) << "Tries  : " << tries << "\n";
        (*log) << "Time   : " << duration << "s\n";
        (*log) << "Speed  : " << static_cast<size_t>(speed) << "/sec\n";
        (*log) << "Password: " << (found ? crackedPassword : "NOT_FOUND") << "\n";
        (*log) << "-----------------------------\n";
    }
}
