#ifndef BASE_SHA1_H_
#define BASE_SHA1_H_
#pragma once

#include <string>

namespace base {

// This function performs SHA-1 operations.
enum {
    SHA1_LENGTH = 20  // Length in bytes of a SHA-1 hash.
};

// Computes the SHA-1 hash of the input string |str| and returns the full
// hash.
std::string SHA1HashString(const std::string& str);
std::string SHA1HashStringToBase16(const std::string& str);
}  // namespace base

#endif  // BASE_SHA1_H_