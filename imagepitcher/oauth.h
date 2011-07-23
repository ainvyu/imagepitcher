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

  virtual bool DoAuth() = 0;
  virtual bool DoRequestToken() = 0;
  virtual bool DoAuthorize() = 0;
  virtual bool DoAccessToken() = 0;
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

  void SetCallbackURL(const std::string& _url) { callbackUrl_ = _url; }
  void SetSignatureMethod(const std::string& _method) { signatureMethod_ = _method; }
  void SetConsumerKey(const std::string& _key) { consumerKey_ = _key; }
  std::string GetConsumerKey() const { return consumerKey_; }
  void SetConsumerSecret(const std::string& _secret) { consumerSecret_ = _secret; }
  std::string GetConsumerSecret() const { return consumerSecret_; }
  void SetOAuthVerifier(const std::string& _verifier) { oAuthVerifier_ = _verifier; }

  const std::string& GetRequestToken() const { return requestToken_; }
  const std::string& GetRequestTokenSecret() const { return requestTokenSecret_; }
  const std::string& GetAccessToken() const { return accessToken_; }
  const std::string& GetAccessSecret() const { return accessTokenSecret_; }

  virtual bool DoAuth();
  virtual bool DoRequestToken();
  virtual bool DoAuthorize();
  virtual bool DoAccessToken();

  void SetRequestToken(const std::string& _requestToken) { requestToken_ = _requestToken; }
  void SetRequestTokenSecret(const std::string& _requestTokenSecret) { requestTokenSecret_ = _requestTokenSecret; }
  void SetAccessToken(const std::string& _accessToken) { accessToken_ = _accessToken; }
  void SetAccessSecret(const std::string& _accessSecret) { accessTokenSecret_= _accessSecret; }
  

protected:
  std::string GenerateTimeStamp();
  std::string GenerateNonce();
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
