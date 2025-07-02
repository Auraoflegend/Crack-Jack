#include "../include/hash_utils.h"
#include <openssl/evp.h>
#include <openssl/md4.h>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cctype>

// Convert binary to hex string
static std::string toHex(const unsigned char* hash, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return oss.str();
}

// Generic EVP hashing
static std::string hashWithEVP(const std::string& input, const EVP_MD* algo) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, algo, nullptr);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);

    return toHex(hash, len);
}

// Hash functions
std::string md5(const std::string& input) {
    return hashWithEVP(input, EVP_md5());
}

std::string sha1(const std::string& input) {
    return hashWithEVP(input, EVP_sha1());
}

std::string sha256(const std::string& input) {
    return hashWithEVP(input, EVP_sha256());
}

std::string sha512(const std::string& input) {
    return hashWithEVP(input, EVP_sha512());
}

// NTLM = MD4(UTF-16LE(password))
std::string ntlm(const std::string& input) {
    std::u16string utf16le(input.begin(), input.end());
    const unsigned char* data = reinterpret_cast<const unsigned char*>(utf16le.data());
    size_t len = utf16le.size() * sizeof(char16_t);

    unsigned char hash[MD4_DIGEST_LENGTH];
    MD4(data, len, hash);

    return toHex(hash, MD4_DIGEST_LENGTH);
}

// Dispatcher
std::string hashString(const std::string& input, const std::string& algo) {
    std::string lower;
    for (char c : algo) lower += std::tolower(static_cast<unsigned char>(c));

    if (lower == "md5")      return md5(input);
    if (lower == "sha1")     return sha1(input);
    if (lower == "sha256")   return sha256(input);
    if (lower == "sha512")   return sha512(input);
    if (lower == "ntlm")     return ntlm(input);
    return "";
}

// Optimizer checks
bool isSalted(HashType type) {
    return type == HASH_BCRYPT || type == HASH_PBKDF2 || type == HASH_SCRYPT;
}

bool isIterated(HashType type) {
    return type == HASH_BCRYPT || type == HASH_PBKDF2 || type == HASH_SCRYPT;
}

bool isRawHash(HashType type) {
    return type == HASH_MD5 || type == HASH_SHA1 || type == HASH_SHA256 ||
           type == HASH_SHA512 || type == HASH_NTLM;
}
