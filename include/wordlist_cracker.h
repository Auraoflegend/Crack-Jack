#ifndef WORDLIST_CRACKER_H
#define WORDLIST_CRACKER_H

#include <string>
#include <ostream>  // ✅ Needed for std::ostream*

// ✅ Add an optional log parameter (default = nullptr)
void crackWithWordlist(const std::string& targetHash,
                       const std::string& algorithm,
                       const std::string& wordlistPath,
                       std::ostream* log = nullptr);

#endif // WORDLIST_CRACKER_H
