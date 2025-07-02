#ifndef WORDLIST_CRACKER_H
#define WORDLIST_CRACKER_H

#include <string>

void crackWithWordlist(const std::string& targetHash, const std::string& algorithm, const std::string& wordlistPath);

#endif
