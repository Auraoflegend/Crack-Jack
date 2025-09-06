Here’s a first draft of a GitHub-ready README.md for your project.
It’s styled with sections developers expect (overview, installation, usage, examples).

# 🔑 CrackJack

**CrackJack** is a lightweight, multi-mode password hash utility written in C++.  
It supports hashing, wordlist- and brute-force–based cracking, benchmarking, and an interactive mode.

---

## ✨ Features
- Generate common hashes (MD5, SHA1, SHA256, SHA512, NTLM).
- Crack hashes using:
  - **Wordlist attack**
  - **Brute-force attack** with customizable charsets, lengths, and multithreading.
- Benchmark algorithms with different parameters.
- Interactive mode for guided use.
- Optional logging & resume support.

---

## ⚙️ Build Instructions

```bash
# Clone repo
git clone https://github.com/yourusername/crackjack.git
cd crackjack

# Build (example with g++)
g++ -std=c++17 -O2 src/main.cpp -o crackjack


Make sure to include the rest of the src/ and include/ files when compiling.

📖 Usage
Usage:
  crackjack --mode hash --input STRING|FILE
  crackjack --mode crack --algo ALGO --hash HASH|--hashes FILE --wordlist FILE [--log output.txt]
  crackjack --mode crack --algo ALGO --hash HASH --brute --charset MASK --min LEN --max LEN --threads N [--log output.txt] [--resume checkpoint.txt]
  crackjack --benchmark --algo ALGO --threads N --charset MASK --length LEN
  crackjack --interactive

🚀 Examples
1. Hash a string
./crackjack --mode hash --input "hello"

2. Hash multiple strings from a file
./crackjack --mode hash --input wordlist.txt

3. Crack with a wordlist
./crackjack --mode crack --algo md5 --hash 5d41402abc4b2a76b9719d911017c592 --wordlist rockyou.txt

4. Brute-force crack
./crackjack --mode crack --algo sha1 --hash 2aae6c35c94fcfb415dbe95f408b9ce91ee846ed \
  --brute --charset ?l?d --min 4 --max 6 --threads 8

5. Benchmark algorithm speed
./crackjack --benchmark --algo sha256 --threads 8 --charset ?l?d?s --length 6

6. Interactive mode
./crackjack --interactive

🔤 Charset Masks

Use masks for brute force charsets:

?l → lowercase letters (a–z)

?u → uppercase letters (A–Z)

?d → digits (0–9)

?s → symbols

Combine: ?l?d = lowercase + digits

📂 Project Structure
crackjack/
├── include/
│   ├── hash_utils.h
│   ├── optimizer.h
│   ├── wordlist_cracker.h
│   ├── brute_force_cracker.h
│   ├── mask_util.h
│   ├── benchmark.h
│   └── interactive.h
├── src/
│   └── main.cpp
└── README.md

🛡️ Disclaimer

This project is intended for educational and research purposes only.
Do not use it for unauthorized password cracking or illegal activities.
The authors take no responsibility for misuse.
