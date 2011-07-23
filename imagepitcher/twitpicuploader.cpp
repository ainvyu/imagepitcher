#include "StdAfx.h"
#include "twitpicuploader.h"

#include "stringutil.h"
#include "httppost.h"

#include "hmac_sha1.h"
#include "urlencode.h"
#include "simplebase64.h"

// TwitPic API Key: a3c7312df01fb8723e6ba4c904fa2f49

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

const string upload_url = "api.twitpic.com";
const string get_url = "/2/upload.xml";

CTwitpicUploader::CTwitpicUploader(CTwitterOAuth& _oauth)
  : oauth_(_oauth)
{
}

CTwitpicUploader::~CTwitpicUploader(void)
{
}

std::string CTwitpicUploader::GenerateTimeStamp() {
  return CStringUtil::Int64ToStrA(time(NULL));
}

std::string CTwitpicUploader::GenerateNonce() {
  return CStringUtil::MakeRadomStringA(32);
}

void CTwitpicUploader::AddPicture( const string& strPath )
{
  filelist_.push_back(strPath);
}

std::string CTwitpicUploader::MakeSignature(const SignatureData& data) {
  stringstream ssBaseString;
  string key = oauth_.GetConsumerSecret() + "&" + oauth_.GetRequestToken();

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

void CTwitpicUploader::DoAuthorize() {
  std::string consumerKey = "8fxwQe0zkggUtgeg9Cw6FQ";
  std::string consumerSecret = "H0r6ELPFd973ylH7e1IwQA8BVdGIsSOAZgwGX2J3SKo";

  oauth_.SetConsumerKey(consumerKey);
  oauth_.SetConsumerSecret(consumerSecret);
  oauth_.SetSignatureMethod("HMAC-SHA1");
  //oauth_.DoAuth();
  oauth_.DoRequestToken();
  oauth_.DoAuthorize();
}

void CTwitpicUploader::DoUpload()
{
  for (auto it = filelist_.cbegin(), _end = filelist_.cend(); it != _end; ++it) {
    const std::string& filePath = (*it);
    const std::string& boundary = MakeBoundary();
    const std::string& content = MakeContent(filePath, boundary);
    
    string nonce = GenerateNonce();
    string timeStamp = GenerateTimeStamp();

    SignatureData data;
    data.method = "POST";
    data.requestUrl = "http://" + upload_url + get_url;
    data.parameter["realm"] = "http://api.twitter.com/";
    data.parameter["oauth_consumer_key"] = oauth_.GetConsumerKey();
    data.parameter["oauth_signature_method"] = "HMAC-SHA1";
    data.parameter["oauth_token"] = oauth_.GetAccessToken();
    data.parameter["oauth_timestamp"] = timeStamp;
    data.parameter["oauth_nonce"] = nonce;
    data.parameter["oauth_version"] = "1.0";    

    CHttpPost post;
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("OAuth realm", "=", "http://api.twitter.com/"));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_consumer_key", "=", oauth_.GetConsumerKey()));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_signature_method", "=", "HMAC-SHA1"));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_token", "=", oauth_.GetAccessToken()));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_timestamp", "=", timeStamp));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_nonce", "=", nonce));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_version", "=", "1.0"));
    post.AddHeader("X-Verify-Credentials-Authorization", 
      PostItem("oauth_signature", "=", MakeSignature(data)));

    post.AddHeader("X-Auth-Service-Provider", 
      PostItem("", "", "https://api.twitter.com/1/account/verify_credentials.json"));

    post.SetURL(upload_url);
    post.SetPath(get_url);
    post.SetBoundary(boundary);
    post.SetContent(content);
    post.SetUserAgent("Image Pitcher");

    if (!post.DoPost())
      break;
    
    const std::string& response = post.GetResponse();
    MessageBoxA(NULL, response.c_str(), "Reponse", MB_OK);
//     pugi::xml_document doc;
//     pugi::xml_parse_result result = doc.load(response.c_str());
//     if (!result)
//       break;
  }
}

const std::vector<char> CTwitpicUploader::GetPictureBinary(const std::string& filePath) {
  std::vector<char> binary;
  binary.reserve((int)fs::file_size(fs::path(filePath)));

  ifstream file;
  file.open(filePath, std::ios::in | std::ios::binary);
  if (file.fail())
    return binary;

  copy(istream_iterator<char> (file), 
       istream_iterator<char> (), back_inserter(binary));

  return binary;
}

std::string CTwitpicUploader::MakeContent(const std::string& filePath, 
                                          const std::string& boundary) {
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
  string fileContentType = GetImageContentType(filePath);
  string fileHeader = 
    CStringUtil::Format("Content-Disposition: file; name=\"media\"; filename=\"%s\"", 
                        fs::path(filePath).filename().string<string>().c_str());
  const vector<char> fileData = GetPictureBinary(filePath);

  ssContents << fileHeader << "\r\n";
  ssContents << "Content-Type: " << fileContentType << "\r\n";
  ssContents << "\r\n";
  std::copy(fileData.begin(), fileData.end(),
            ostream_iterator<char> (ssContents));
  ssContents << "\r\n";

  ssContents << footer << "\r\n";

  return ssContents.str();
}

std::string CTwitpicUploader::GetImageContentType(const std::string& filePath) {
  boost::filesystem::path image_path(filePath);
  string ext = image_path.extension().string();

  if (".jpg" == ext) {
    return "image/jpeg";
  }
  else if (".gif" == ext) {
    return "image/gif";
  }
  else if (".png" == ext) {
    return "image/png";
  }

  return "image/jpeg";
}

std::string CTwitpicUploader::MakeBoundary() {
  return CStringUtil::MakeRadomStringA(16);
}