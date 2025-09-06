#include "../include/resume_manager.h"
#include <fstream>
#include <sstream>

// Format: LENGTH INDEX
bool loadResumeCheckpoint(const std::string& path, uint64_t& startIndex, int& length) {
    std::ifstream infile(path);
    if (!infile.is_open()) return false;

    std::string line;
    if (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (iss >> length >> startIndex) {
            return true;
        }
    }
    return false;
}

void saveResumeCheckpoint(const std::string& path, uint64_t index, int length) {
    std::ofstream outfile(path, std::ios::trunc);
    if (outfile.is_open()) {
        outfile << length << " " << index << "\n";
    }
}
