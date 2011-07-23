#include "StdAfx.h"
#include "oauth.h"

#include "stringutil.h"
#include "httppost.h"

#include "hmac_sha1.h"
#include "urlencode.h"
#include "simplebase64.h"

using namespace std;

CTwitterOAuth::CTwitterOAuth( void )
  : oAuthVersion_("1.0")
{
}

CTwitterOAuth::~CTwitterOAuth( void )
{

}

std::string CTwitterOAuth::GenerateTimeStamp() {
  return CStringUtil::Int64ToStrA(time(NULL));
}

std::string CTwitterOAuth::GenerateNonce() {
  return CStringUtil::MakeRadomStringA(32);
}

bool CTwitterOAuth::DoAuth()
{
  // 1. POST Request Token
  DoRequestToken();
  // 2. POST Authorize
  DoAuthorize();
  // 3. POST Access Token
  DoAccessToken();

  return true;
}

bool CTwitterOAuth::DoRequestToken()
{
  CHttpPost post;
  post.SetURL(oauthurl::REQUEST_URL);
  post.SetPort(oauthurl::REQUEST_PORT);
  post.SetPath(oauthurl::REQUEST_TOKEN);
  post.SetUserAgent("Image Pitcher");

  string nonce = GenerateNonce();
  string timeStamp = GenerateTimeStamp();

  // oauth_nonce는 개발자가 임의로 생성
  //post.RegisterSectionSeparator("OAuth", " ");

  // Make Signature
  SignatureData data;
  data.method = "POST";
  data.requestUrl = "http://" + oauthurl::REQUEST_URL + oauthurl::REQUEST_TOKEN;
  data.parameter["oauth_consumer_key"] = consumerKey_;
  data.parameter["oauth_nonce"] = nonce;
  data.parameter["oauth_signature_method"] = signatureMethod_;
  data.parameter["oauth_timestamp"] = timeStamp;
  data.parameter["oauth_version"] = oAuthVersion_;

  if (!callbackUrl_.empty())
    post.AddHeader("Authorization", PostItem("oauth_callback", "=", callbackUrl_));

  post.AddHeader("Authorization", 
    PostItem("OAuth oauth_consumer_key", "=", consumerKey_));  
  post.AddHeader("Authorization", 
    PostItem("oauth_nonce", "=", nonce)); 
  post.AddHeader("Authorization", 
    PostItem("oauth_signature_method", "=", signatureMethod_));
  post.AddHeader("Authorization", 
    PostItem("oauth_timestamp", "=", timeStamp));
  
  post.AddHeader("Authorization",
    PostItem("oauth_signature", "=", makeSignature(data)));
  post.AddHeader("Authorization", 
    PostItem("oauth_version", "=", oAuthVersion_));

  if (!post.DoPost())
    return false;

  // oauth_token=zd9BdHtPAOhhaJyo4xhfFTAvDtMG5dWefLTiKMXHrk&oauth_token_secret=pSrAL9TRqYQ18WJuOfXoeek2tUdayglU5MCdKfMXCY&oauth_callback_confirmed=true
  // Token Parsing  
  unordered_map<string, string> response_map;
  const string& response = post.GetResponse();
  int pos_start_content = response.find("\r\n\r\n") + 4;
  string content = response.substr(pos_start_content);

  vector<string> items = CStringUtil::Split(content, "&");
  for (auto it = items.cbegin(), end = items.cend(); it != end; ++it) {
    const string& item = (*it);
    vector<string> split_item = CStringUtil::Split(item, "=");
    string& key = split_item[0];
    string& val = split_item[1];

    response_map.insert(unordered_map<string, string>::value_type(key, val));
  }

  requestToken_ = response_map["oauth_token"];
  requestTokenSecret_ = response_map["oauth_token_secret"];

  return true;
}

std::string CTwitterOAuth::makeSignature(const SignatureData& data) const {
  stringstream ssBaseString;
  string key = consumerSecret_ + "&" + requestToken_;

  // Build base string.
  // base string은 http header로 보내는 파라미터이다.
  // [http method] & [url] & [parameter] 형식으로 생성한다.
  string requestUrl = data.requestUrl;
  // Sorted params
  string parameter;
  for (auto it = data.parameter.cbegin(), _end = data.parameter.cend();
      it != _end; ++it) {
    const std::string& key = (*it).first;
    const std::string& val = (*it).second;

    parameter += key + "=" + val + "&";
  }

  if (!parameter.empty()) {
    // 맨 마지막 &를 지움.
    parameter.erase(parameter.end()-1);
  }

  ssBaseString << data.method << "&";
  ssBaseString << CUrlEncode::URLEncode(requestUrl.c_str()) << "&";
  ssBaseString << CUrlEncode::URLEncode(parameter.c_str());

  std::string hmacString = base::HMACSHA1HashString(ssBaseString.str(), key); 

  return CUrlEncode::URLEncode(CSimpleBase64::Encode(hmacString));
}

bool CTwitterOAuth::DoAuthorize()
{
  string authorizeUrl = "http://" + oauthurl::REQUEST_URL + oauthurl::AUTHORIZE + "?oauth_token=" + requestToken_;
  ShellExecuteA(NULL, "open", authorizeUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);

  return true;
}

bool CTwitterOAuth::DoAccessToken()
{
  CHttpPost post;
  post.SetURL(oauthurl::REQUEST_URL);
  post.SetPort(oauthurl::REQUEST_PORT);
  post.SetPath(oauthurl::ACCESS_TOKEN);
  post.SetUserAgent("Image Pitcher");

  string nonce = GenerateNonce();
  string timeStamp = GenerateTimeStamp();

  // oauth_nonce는 개발자가 임의로 생성
  //post.RegisterSectionSeparator("OAuth", " ");

  // Make Signature
  SignatureData data;
  data.method = "POST";
  data.requestUrl = "http://" + oauthurl::REQUEST_URL + oauthurl::ACCESS_TOKEN;
  data.parameter["oauth_consumer_key"] = consumerKey_;
  data.parameter["oauth_nonce"] = nonce;
  data.parameter["oauth_signature_method"] = signatureMethod_;
  data.parameter["oauth_token"] = requestToken_ ;
  data.parameter["oauth_timestamp"] = timeStamp;
  data.parameter["oauth_verifier"] = oAuthVerifier_;
  data.parameter["oauth_version"] = oAuthVersion_;

  if (!callbackUrl_.empty())
    post.AddHeader("Authorization", PostItem("oauth_callback", "=", callbackUrl_));

  post.AddHeader("Authorization", 
    PostItem("OAuth oauth_consumer_key", "=", consumerKey_));
  post.AddHeader("Authorization", 
    PostItem("oauth_nonce", "=", nonce)); 
  post.AddHeader("Authorization", 
    PostItem("oauth_signature_method", "=", signatureMethod_));
  post.AddHeader("Authorization", 
    PostItem("oauth_token", "=", requestToken_));
  post.AddHeader("Authorization", 
    PostItem("oauth_timestamp", "=", timeStamp));
  post.AddHeader("Authorization",
    PostItem("oauth_signature", "=", makeSignature(data)));
  post.AddHeader("Authorization", 
    PostItem("oauth_verifier", "=", oAuthVerifier_));
  post.AddHeader("Authorization", 
    PostItem("oauth_version", "=", oAuthVersion_));

  if (!post.DoPost())
    return false;

  // oauth_token=14897835-eWIBYtkp6sA2HBcNEZS3Wx2tfBRwOS1c3qhDporIl&oauth_token_secret=ucKjAuDC08Rqfpo28bik3HoyRpmmwT0rv84WE1E7j0&user_id=14897835&screen_name=nving
  // Token Parsing 
  unordered_map<string, string> response_map;
  const string& response = post.GetResponse();
  int pos_start_content = response.find("\r\n\r\n") + 4;
  string content = response.substr(pos_start_content);

  vector<string> items = CStringUtil::Split(content, "&");
  for (auto it = items.cbegin(), end = items.cend(); it != end; ++it) {
    const string& item = (*it);
    vector<string> split_item = CStringUtil::Split(item, "=");
    string& key = split_item[0];
    string& val = split_item[1];

    response_map.insert(unordered_map<string, string>::value_type(key, val));
  }

  accessToken_ = response_map["oauth_token"];
  accessTokenSecret_ = response_map["oauth_token_secret"];

  return true;
}