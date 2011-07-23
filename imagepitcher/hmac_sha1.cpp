#include "StdAfx.h"

#include "hmac_sha1.h"
#include "sha1.h"

using namespace std;

namespace base {

class CHMAC_SHA1
{
public:
  enum {
    SHA1_DIGEST_LENGTH      = 20,
    SHA1_BLOCK_SIZE         = 64,
    HMAC_BUF_LEN            = 4096
  };
  CHMAC_SHA1() {}
  ~CHMAC_SHA1() {}

  std::string GetHmacSHA1HashString(const std::string& text,            
                                    const std::string& key);

private:
  char i_key_pad_[64];
  char o_key_pad_[64];
};

std::string CHMAC_SHA1::GetHmacSHA1HashString(const std::string& text, 
                                              const std::string& key)
{
  char sha1_key[SHA1_BLOCK_SIZE] = {0};

  if (key.length() > SHA1_BLOCK_SIZE) {
    std::string sha1Str = base::SHA1HashString(key);
    memcpy_s(sha1_key, SHA1_BLOCK_SIZE, sha1Str.c_str(), sha1Str.size());
  }
  else {
    memcpy_s(sha1_key, SHA1_BLOCK_SIZE, key.c_str(), key.size());
  }

  for (int i = 0; i < SHA1_BLOCK_SIZE; ++i) {
    i_key_pad_[i] = 0x36^sha1_key[i];               
    o_key_pad_[i] = 0x5c^sha1_key[i];
  }

  vector<char> buf1;
  buf1.reserve(HMAC_BUF_LEN);
  buf1.insert(buf1.end(), i_key_pad_, i_key_pad_ + SHA1_BLOCK_SIZE);
  buf1.insert(buf1.end(), text.begin(), text.end());
  string buf1_hash = base::SHA1HashString(string(buf1.begin(), buf1.end()));

  vector<char> buf2;
  buf2.reserve(HMAC_BUF_LEN);
  buf2.insert(buf2.end(), o_key_pad_, o_key_pad_ + SHA1_BLOCK_SIZE);
  buf2.insert(buf2.end(), buf1_hash.begin(), buf1_hash.end());


  string hmac_sha1 = base::SHA1HashString(string(buf2.begin(), buf2.end()));
  return hmac_sha1;
}

std::string HMACSHA1HashString(const std::string& text, 
  const std::string& key) {
    CHMAC_SHA1 hmac_sha1;
    return hmac_sha1.GetHmacSHA1HashString(text, key);
}

std::string HMACSHA1HashStringToBase16(const std::string& text, 
  const std::string& key) {
    std::string hash = HMACSHA1HashString(text, key);
    static char const zEncode[] = "0123456789abcdef";

    std::string ret;
    ret.resize(40);

    int j = 0;
    for (int i = 0; i < 20; i ++) {
      int a = hash[i];
      ret[j++] = zEncode[(a>>4) & 0xf];
      ret[j++] = zEncode[a & 0xf];
    }
    return ret;
}

}