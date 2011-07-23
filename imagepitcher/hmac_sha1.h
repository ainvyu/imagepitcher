#pragma once

/*
  HMAC (Hash-based Message Authentication Code) is a specific construction for 
  calculating a message authentication code (MAC) involving 
  a cryptographic hash function in combination with a secret key.

                          - from Wikipedia(http://en.wikipedia.org/wiki/HMAC)
*/

namespace base {

// This function performs SHA-1 operations.
enum {
  HMAC_SHA1_LENGTH = 20  // Length in bytes of a SHA-1 hash.
};

// Computes the SHA-1 hash of the input string |str| and returns the full
// hash.
std::string HMACSHA1HashString(const std::string& text, const std::string& key);
std::string HMACSHA1HashStringToBase16(const std::string& text, const std::string& key);
}  // namespace base


