#include "../include/mask_util.h"
#include <unordered_map>

std::string expandMaskCharset(const std::string& mask) {
    std::unordered_map<char, std::string> lookup = {
        { 'l', "abcdefghijklmnopqrstuvwxyz" },
        { 'u', "ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
        { 'd', "0123456789" },
        { 's', "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" },
        { 'a', "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" }
    };

    std::string result;
    for (size_t i = 0; i < mask.length(); ++i) {
        if (mask[i] == '?' && i + 1 < mask.length()) {
            char type = mask[i + 1];
            auto it = lookup.find(type);
            if (it != lookup.end()) {
                result += it->second;
            }
            i++;  // Skip next char
        } else {
            result += mask[i];  // literal character
        }
    }

    return result;
}
