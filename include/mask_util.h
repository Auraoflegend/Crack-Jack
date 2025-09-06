#ifndef MASK_UTIL_H
#define MASK_UTIL_H

#include <string>
#include <cstdint>  // ✅ for uint64_t


// Expands a Hashcat-style mask like "?a?l?d" into a full charset
std::string expandMaskCharset(const std::string& mask);

// Converts a numeric index to a string candidate using charset and length
std::string index_to_candidate(uint64_t index, const std::string& charset, int length);  // ✅ ADDED

#endif // MASK_UTIL_H
