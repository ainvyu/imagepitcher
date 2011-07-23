#include "StdAfx.h"

#include "sha1.h"

namespace base {

// Implementation of SHA-1. Only handles data in byte-sized blocks,
// which simplifies the code a fair bit.

// Identifier names follow notation in FIPS PUB 180-3, where you'll
// also find a description of the algorithm:
// http://csrc.nist.gov/publications/fips/fips180-3/fips180-3_final.pdf

// Usage example:
//
// SecureHashAlgorithm sha;
// while(there is data to hash)
//   sha.Update(moredata, size of data);
// sha.Final();
// memcpy(somewhere, sha.Digest(), 20);
//
// to reuse the instance of sha, call sha.Init();

// TODO(jhawkins): Replace this implementation with a per-platform
// implementation using each platform's crypto library.  See
// http://crbug.com/47218

class SecureHashAlgorithm {
public:
    SecureHashAlgorithm() { Init(); }

    static const int kDigestSizeBytes;

    void Init();
    void Update(const void* data, size_t nbytes);
    void Final();

    // 20 bytes of message digest.
    const unsigned char* Digest() const {
        return reinterpret_cast<const unsigned char*>(H);
    }

private:
    void Pad();
    void Process();

    UINT A, B, C, D, E;

    UINT H[5];

    union {
        UINT32 W[80];
        UINT8 M[64];
    };

    UINT cursor;
    UINT l;
};

static inline UINT f(UINT t, UINT B, UINT C, UINT D) {
    if (t < 20) {
        return (B & C) | ((~B) & D);
    } else if (t < 40) {
        return B ^ C ^ D;
    } else if (t < 60) {
        return (B & C) | (B & D) | (C & D);
    } else {
        return B ^ C ^ D;
    }
}

static inline UINT S(UINT n, UINT X) {
    return (X << n) | (X >> (32-n));
}

static inline UINT K(UINT t) {
    if (t < 20) {
        return 0x5a827999;
    } else if (t < 40) {
        return 0x6ed9eba1;
    } else if (t < 60) {
        return 0x8f1bbcdc;
    } else {
        return 0xca62c1d6;
    }
}

static inline void swapends(UINT* t) {
    *t = ((*t & 0xff000000) >> 24) |
         ((*t & 0xff0000) >> 8) |
         ((*t & 0xff00) << 8) |
         ((*t & 0xff) << 24);
}

const int SecureHashAlgorithm::kDigestSizeBytes = 20;

void SecureHashAlgorithm::Init() {
    cursor = 0;
    l = 0;
    H[0] = 0x67452301;
    H[1] = 0xefcdab89;
    H[2] = 0x98badcfe;
    H[3] = 0x10325476;
    H[4] = 0xc3d2e1f0;
}

void SecureHashAlgorithm::Final() {
    Pad();
    Process();

    for (int t = 0; t < 5; ++t)
        swapends(&H[t]);
}

void SecureHashAlgorithm::Update(const void* data, size_t nbytes) {
    const UINT8* d = reinterpret_cast<const UINT8*>(data);
    while (nbytes--) {
        M[cursor++] = *d++;
        if (cursor >= 64)
            Process();
        l += 8;
    }
}

void SecureHashAlgorithm::Pad() {
    M[cursor++] = 0x80;

    if (cursor > 64-8) {
        // pad out to next block
        while (cursor < 64)
            M[cursor++] = 0;

        Process();
    }

    while (cursor < 64-4)
        M[cursor++] = 0;

    M[64-4] = (l & 0xff000000) >> 24;
    M[64-3] = (l & 0xff0000) >> 16;
    M[64-2] = (l & 0xff00) >> 8;
    M[64-1] = (l & 0xff);
}

void SecureHashAlgorithm::Process() {
    UINT t;

    // Each a...e corresponds to a section in the FIPS 180-3 algorithm.

    // a.
    //
    // W and M are in a union, so no need to memcpy.
    // memcpy(W, M, sizeof(M));
    for (t = 0; t < 16; ++t)
        swapends(&W[t]);

    // b. Extend the sixteen 32-bit words into eighty 32-bit words:
    for (t = 16; t < 80; ++t)
        W[t] = S(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

    // c. Initialize hash value for this chunk:
    A = H[0];
    B = H[1];
    C = H[2];
    D = H[3];
    E = H[4];

    // d.
    for (t = 0; t < 80; ++t) {
        UINT TEMP = S(5, A) + f(t, B, C, D) + E + W[t] + K(t);
        E = D;
        D = C;
        C = S(30, B);
        B = A;
        A = TEMP;
    }

    // e. Add this chunk's hash to result so far:
    H[0] += A;
    H[1] += B;
    H[2] += C;
    H[3] += D;
    H[4] += E;

    cursor = 0;
}

std::string SHA1HashString(const std::string& str) {
    SecureHashAlgorithm sha;
    sha.Update(str.c_str(), str.length());
    sha.Final();
    std::string out(reinterpret_cast<const char*>(sha.Digest()),
        SecureHashAlgorithm::kDigestSizeBytes);
    return out;
}

std::string SHA1HashStringToBase16(const std::string& str) {
  const std::string hash = SHA1HashString(str);

  static char const zEncode[] = "0123456789abcdef";

  std::string ret;
  ret.resize(40);

  int j = 0;
  for (int i = 0; i < 20; i ++) {
    int a = hash[i];
    ret[j++] = zEncode[(a>>4)&0xf];
    ret[j++] = zEncode[a & 0xf];
  }
  return ret;
}

}  // namespace base