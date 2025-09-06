#pragma once

#include <string>
#include "batch_hash.h"  // For HashAlgorithm enum

// === Individual Hash Functions ===
std::string md5(const std::string& input);
std::string sha1(const std::string& input);
std::string sha256(const std::string& input);
std::string sha512(const std::string& input);
std::string ntlm(const std::string& input);

// === Enum-based Dispatch ===
std::string computeHash(HashAlgorithm algo, const std::string& input);

// === String-based Dispatch (for legacy CLI or benchmark) ===
std::string hashString(const std::string& input, const std::string& algorithm);
