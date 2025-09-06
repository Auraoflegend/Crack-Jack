#include "../include/hash_utils.h"

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/md4.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <codecvt>
#include <locale>

std::string bytesToHex(const unsigned char* bytes, size_t length) {
    std::stringstream ss;
    for (size_t i = 0; i < length; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
    return ss.str();
}

std::string md5(const std::string& input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)input.c_str(), input.size(), hash);
    return bytesToHex(hash, MD5_DIGEST_LENGTH);
}

std::string sha1(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)input.c_str(), input.size(), hash);
    return bytesToHex(hash, SHA_DIGEST_LENGTH);
}

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.size(), hash);
    return bytesToHex(hash, SHA256_DIGEST_LENGTH);
}

std::string sha512(const std::string& input) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512((const unsigned char*)input.c_str(), input.size(), hash);
    return bytesToHex(hash, SHA512_DIGEST_LENGTH);
}

std::string ntlm(const std::string& input) {
    // Convert to UTF-16LE
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    std::wstring wide = conv.from_bytes(input);
    std::vector<unsigned char> buffer(wide.size() * 2);

    for (size_t i = 0; i < wide.size(); ++i) {
        buffer[2 * i] = wide[i] & 0xFF;
        buffer[2 * i + 1] = (wide[i] >> 8) & 0xFF;
    }

    unsigned char hash[MD4_DIGEST_LENGTH];
    MD4(buffer.data(), buffer.size(), hash);
    return bytesToHex(hash, MD4_DIGEST_LENGTH);
}

// === Dispatch via enum ===
std::string computeHash(HashAlgorithm algo, const std::string& input) {
    switch (algo) {
        case HashAlgorithm::MD5: return md5(input);
        case HashAlgorithm::SHA1: return sha1(input);
        case HashAlgorithm::SHA256: return sha256(input);
        case HashAlgorithm::SHA512: return sha512(input);
        case HashAlgorithm::NTLM: return ntlm(input);
        default: return "";
    }
}

// === Dispatch via algorithm string (for CLI) ===
std::string hashString(const std::string& input, const std::string& algorithm) {
    if (algorithm == "md5") return md5(input);
    if (algorithm == "sha1") return sha1(input);
    if (algorithm == "sha256") return sha256(input);
    if (algorithm == "sha512") return sha512(input);
    if (algorithm == "ntlm") return ntlm(input);
    return "";
}
