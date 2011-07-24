#include "StdAfx.h"
#include "twitpicuploader2.h"

#include "stringutil.h"
#include "httppost.h"

#include "hmac_sha1.h"
#include "urlencode.h"
#include "simplebase64.h"

// TwitPic API Key: a3c7312df01fb8723e6ba4c904fa2f49

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

const string uploadUrl = "api.twitpic.com";
const string getUrl = "/2/upload.xml";
const string twitterVerifyCredentialsApiUrl = 
  "https://api.twitter.com/1/account/verify_credentials.json";

CTwitpicUploader2::CTwitpicUploader2(CTwitterOAuth& _oauth)
  : CTwitpicUploader(_oauth)
{
}


CTwitpicUploader2::~CTwitpicUploader2(void)
{
}

std::string CTwitpicUploader2::makeContent(const std::string& filePath, 
                                           const std::string& boundary)
{
  const string& header = "--" + boundary;
  const string& footer = "--" + boundary + "--";

  stringstream ssContents;

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"key\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << apiKey_ << "\r\n";

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"message\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << tweetMsg_ << "\r\n";

  ssContents << header << "\r\n";
  string fileContentType = getImageContentType(filePath);
  string fileHeader = 
    CStringUtil::Format("Content-Disposition: file; name=\"media\"; filename=\"%s\"", 
    fs::path(filePath).filename().string<string>().c_str());
  const vector<char> fileData = getPictureBinary(filePath);

  ssContents << fileHeader << "\r\n";
  ssContents << "Content-Type: " << fileContentType << "\r\n";
  ssContents << "\r\n";
  std::copy(fileData.begin(), fileData.end(),
    ostream_iterator<char> (ssContents));
  ssContents << "\r\n";

  ssContents << footer << "\r\n";

  return ssContents.str();
}

void CTwitpicUploader2::addCustomHeader( CHttpPost& post ) {
  //     SignatureData data;
  //     data.method = "POST";
  //     data.requestUrl = "http://" + uploadUrl + getUrl;
  //     data.parameter["realm"] = "http://api.twitter.com/";
  //     data.parameter["oauth_consumer_key"] = OAuth.getConsumerKey();
  //     data.parameter["oauth_signature_method"] = "HMAC-SHA1";
  //     data.parameter["oauth_token"] = OAuth.getAccessToken();
  //     data.parameter["oauth_timestamp"] = timeStamp;
  //     data.parameter["oauth_nonce"] = nonce;
  //     data.parameter["oauth_version"] = "1.0";    

  SignatureData data;
  data.method = "GET";
  data.requestUrl = twitterVerifyCredentialsApiUrl;
  data.parameter["oauth_consumer_key"] = OAuth.getConsumerKey();
  data.parameter["oauth_consumer_secret"] = OAuth.getConsumerSecret();
  data.parameter["oauth_token"] = OAuth.getAccessToken();
  data.parameter["oauth_token_secret"] = OAuth.getAccessSecret();
  data.parameter["oauth_timestamp"] = timeStamp;
  data.parameter["oauth_nonce"] = nonce;
  data.parameter["oauth_version"] = "1.0";

  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("OAuth realm", "=", "http://api.twitter.com/"));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_consumer_key", "=", OAuth.getConsumerKey()));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_signature_method", "=", "HMAC-SHA1"));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_token", "=", OAuth.getAccessToken()));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_timestamp", "=", timeStamp));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_nonce", "=", nonce));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_version", "=", "1.0"));
  post.addHeader("X-Verify-Credentials-Authorization", 
    PostItem("oauth_signature", "=", makeSignature(data)));

  post.addHeader("X-Auth-Service-Provider", 
    PostItem("", "", twitterVerifyCredentialsApiUrl));
}
