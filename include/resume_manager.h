#pragma once

#include <string>
#include <fstream>
#include <cstdint>

struct ResumeState {
    int length = 0;
    uint64_t index = 0;

    ResumeState() = default;

    ResumeState(int len, uint64_t idx) : length(len), index(idx) {}

    bool load(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return false;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));
        file.read(reinterpret_cast<char*>(&index), sizeof(index));
        return file.good();
    }

    void save(const std::string& path) const {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);
        if (!file) return;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(&index), sizeof(index));
    }
};
