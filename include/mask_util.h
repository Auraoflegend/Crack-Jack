#ifndef MASK_UTIL_H
#define MASK_UTIL_H

#include <string>

// Expands a Hashcat-style mask like "?a?l?d" into a full charset
std::string expandMaskCharset(const std::string& mask);

#endif // MASK_UTIL_H
