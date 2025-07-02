#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <string>

// Hash functions
std::string md5(const std::string&);
std::string sha1(const std::string&);
std::string sha256(const std::string&);
std::string sha512(const std::string&);
std::string ntlm(const std::string&);

// Unified hashing interface
std::string hashString(const std::string& input, const std::string& algo);

// Define internal hash types
enum HashType {
    HASH_UNKNOWN = 0,
    HASH_MD5,
    HASH_SHA1,
    HASH_SHA256,
    HASH_SHA512,
    HASH_NTLM,
    HASH_BCRYPT,
    HASH_PBKDF2,
    HASH_SCRYPT
};

// Optimizer helper functions
bool isSalted(HashType type);
bool isIterated(HashType type);
bool isRawHash(HashType type);

#endif
