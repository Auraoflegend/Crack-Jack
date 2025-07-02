cmake_minimum_required(VERSION 3.10)
project(crackjack)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD")

# Include dirs
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OPENSSL_INCLUDE_DIR})  # ✅ This adds openssl/md4.h visibility

# Link dirs
link_directories(${OPENSSL_LIB_DIR})

# Collect sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Target
add_executable(crackjack ${SOURCES})

# Link OpenSSL library (crypto only)
target_link_libraries(crackjack libcrypto)
