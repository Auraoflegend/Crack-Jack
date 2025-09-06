#pragma once

#include <vector>
#include <string>
#include <memory>

enum class HashAlgorithm {
    MD5,
    SHA1,
    SHA256,
    SHA512,
    NTLM
};

class HashEngine {
public:
    virtual ~HashEngine() = default;
    virtual std::string hashString(const std::string& input) = 0;
    virtual std::vector<std::string> hashBatch(const std::vector<std::string>& inputs) = 0;
};

std::unique_ptr<HashEngine> createHashEngine(HashAlgorithm algo);
