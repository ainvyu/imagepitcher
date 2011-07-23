#pragma once

struct lexicographical_compair
{ 
  bool operator()(const std::string& _Left, const std::string& _Right) const 
  { 
    return (_Left < _Right);
  } 
}; 

struct SignatureData {
  std::string method;
  std::string requestUrl;
  std::map<std::string, std::string, lexicographical_compair> parameter;
};

class IOAuth
{
public:
  virtual ~IOAuth() {}

  virtual bool doAuth() = 0;
  virtual bool doRequestToken() = 0;
  virtual bool doAuthorize() = 0;
  virtual bool doAccessToken() = 0;
};

namespace oauthurl {
static const std::string& REQUEST_URL    = "api.twitter.com";
static const std::string& REQUEST_PORT   = "80";
static const std::string& REQUEST_TOKEN  = "/oauth/request_token";
static const std::string& AUTHORIZE      = "/oauth/authorize";
static const std::string& ACCESS_TOKEN   = "/oauth/access_token";
}

class CTwitterOAuth: public IOAuth
{
public:
  CTwitterOAuth(void);
  virtual ~CTwitterOAuth(void);

  void setCallbackURL(const std::string& _url) { callbackUrl_ = _url; }
  void setSignatureMethod(const std::string& _method) { signatureMethod_ = _method; }
  void setConsumerKey(const std::string& _key) { consumerKey_ = _key; }
  std::string getConsumerKey() const { return consumerKey_; }
  void setConsumerSecret(const std::string& _secret) { consumerSecret_ = _secret; }
  std::string getConsumerSecret() const { return consumerSecret_; }
  void setOAuthVerifier(const std::string& _verifier) { oAuthVerifier_ = _verifier; }

  const std::string& getRequestToken() const { return requestToken_; }
  const std::string& getRequestTokenSecret() const { return requestTokenSecret_; }
  const std::string& getAccessToken() const { return accessToken_; }
  const std::string& getAccessSecret() const { return accessTokenSecret_; }

  virtual bool doAuth();
  virtual bool doRequestToken();
  virtual bool doAuthorize();
  virtual bool doAccessToken();

  void setRequestToken(const std::string& _requestToken) { requestToken_ = _requestToken; }
  void setRequestTokenSecret(const std::string& _requestTokenSecret) { requestTokenSecret_ = _requestTokenSecret; }
  void setAccessToken(const std::string& _accessToken) { accessToken_ = _accessToken; }
  void setAccessSecret(const std::string& _accessSecret) { accessTokenSecret_= _accessSecret; }
  

protected:
  std::string generateTimeStamp();
  std::string generateNonce();
  std::string makeSignature(const SignatureData& data) const;

private:
  std::string oAuthVersion_;
  std::string signatureMethod_;
  std::string callbackUrl_;

  std::string consumerKey_;
  std::string consumerSecret_;

  /// Result:
  /// Request token
  std::string requestToken_;
  std::string requestTokenSecret_;

  /// Authorize
  std::string oAuthVerifier_;

  /// Access token
  std::string accessToken_;
  std::string accessTokenSecret_;
};
