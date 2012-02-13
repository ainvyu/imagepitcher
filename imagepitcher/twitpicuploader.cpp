#include "StdAfx.h"
#include "twitpicuploader.h"

#include "stringutil.h"
#include "twitpicpost.h"

#include "hmac_sha1.h"
#include "urlencode.h"
#include "simplebase64.h"

// TwitPic API Key: a3c7312df01fb8723e6ba4c904fa2f49

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

CTwitpicUploader::CTwitpicUploader(CTwitterOAuth& _oauth)
  : OAuth(_oauth)
  , progressPercent(0)
  , completeFlag(false)
  , failFlag(false)
{
}

CTwitpicUploader::~CTwitpicUploader(void)
{
}

std::string CTwitpicUploader::generateTimeStamp() {
  return CStringUtil::Int64ToStrA(time(NULL));
}

std::string CTwitpicUploader::generateNonce() {
  return CStringUtil::MakeRadomStringA(32);
}

void CTwitpicUploader::addPicture( const string& strPath )
{
  fileList.push_back(strPath);
}

std::string CTwitpicUploader::makeSignature(const SignatureData& data) {
  stringstream ssBaseString;
  string key = OAuth.getConsumerSecret() + "&" + OAuth.getRequestToken();

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

void CTwitpicUploader::doAuthorize() {
  OAuth.doRequestToken();
  OAuth.doAuthorize();
}

bool CTwitpicUploader::doUpload()
{
  int count = fileList.size();
  for (auto it = fileList.cbegin(), _end = fileList.cend(); it != _end; ++it) {
    const std::string& filePath = (*it);
    const std::string& boundary = makeBoundary();
    const std::string& content = makeContent(filePath, boundary);
    
    nonce = generateNonce();
    timeStamp = generateTimeStamp();

    TwitpicPost post;
    addCustomHeaderTo(post);

    post.setURL(getUploadUrl());
    post.setPath(getUploadPath());
    post.setBoundary(boundary);
    post.setContent(content);
    post.setUserAgent("Image Pitcher");
    post.setProgressCallback([this] (const int& percent) {
      this->setProgressPercent(percent);
    });

    // post ratio is 90/100
    if (!post.doPost()) {
      failFlag = true;
      return false;
    }

    progressPercent = 50 / count;
    
    // post ratio is 10/100
    const std::string& response = post.getResponse();
    if (!parseResponse(response)) {
      failFlag = true;
      return false;
    }

    progressPercent = 100 / count;
  }

  completeFlag = true;

  return true;
}

bool CTwitpicUploader::parseResponse(const std::string response) {
  int pos_start_content = response.find("\r\n\r\n") + 4;
  int pos_start_xml = response.find("<?xml", pos_start_content);
  int pos_end_xml = response.find("0\r\n\r\n", pos_start_xml);
  string content = response.substr(pos_start_xml, pos_end_xml - pos_start_xml);

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load(content.c_str());
  if (!result)
    return false;

  pugi::xml_node imageNode = doc.child("image");
  if (imageNode.empty())
    return false;

  pugi::xml_node urlNode = imageNode.child("url");
  if (urlNode.empty())
    return false;

  pugi::xml_node urlNodeChild = urlNode.first_child();

  reponseUrlList.push_back(urlNodeChild.value());

  return true;
}

void CTwitpicUploader::addCustomHeaderTo(CHttpPost& post) {
  // nothing
}

const std::vector<char> CTwitpicUploader::getPictureBinary(const std::string& filePath) {
  std::vector<char> binary;

  tstring filePathW = CONVERT_LOCAL(filePath);
  binary.reserve((int)fs::file_size( fs::path(filePathW)) );

  ifstream file;
  file.open(filePathW, std::ios::in | std::ios::binary);
  if (file.fail())
    return binary;

  copy(istreambuf_iterator<char>(file), 
       istreambuf_iterator<char>(), back_inserter(binary));

  return binary;
}

string CTwitpicUploader::makeContent(const std::string& filePath, 
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
  ssContents << "Content-Disposition: form-data; name=\"consumer_token\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << OAuth.getConsumerKey() << "\r\n";

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"consumer_secret\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << OAuth.getConsumerSecret() << "\r\n";

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"oauth_token\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << OAuth.getAccessToken() << "\r\n";

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"oauth_secret\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << OAuth.getAccessSecret() << "\r\n";

  ssContents << header << "\r\n";
  ssContents << "Content-Disposition: form-data; name=\"message\"" << "\r\n";
  ssContents << "\r\n";
  ssContents << tweetMsg_ << "\r\n";

  ssContents << header << "\r\n";
  string fileContentType = getImageContentType(filePath);
  string fileHeader = 
    CStringUtil::Format("Content-Disposition: file; name=\"media\"; filename=\"%s\"", 
    fs::path(CONVERT_LOCAL(filePath)).filename().string<string>().c_str());
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

std::string CTwitpicUploader::getImageContentType(const std::string& filePath) {
  boost::filesystem::path image_path(CONVERT_LOCAL(filePath));
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

std::string CTwitpicUploader::makeBoundary() {
  return CStringUtil::MakeRadomStringA(16);
}