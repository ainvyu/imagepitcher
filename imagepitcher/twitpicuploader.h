#pragma once

#include "oauth.h"

class CHttpPost;
class CTwitterOAuth;

class CTwitpicUploader
{
public:
  CTwitpicUploader(CTwitterOAuth& _oauth);
  virtual ~CTwitpicUploader(void);

  void setAPIKey(const std::string& _apiKey) { apiKey_ = _apiKey; }
  void setTweetMessage(const std::string& _twtMsg) { tweetMsg_ = _twtMsg; }

  void addPicture(const std::string& strPath);
  virtual void doUpload();
  void doAuthorize();

protected:
  virtual std::string makeContent(const std::string& filePath, 
                                  const std::string& boundary);
  std::string makeBoundary();
  std::string getImageContentType(const std::string& path);
  const std::vector<char> getPictureBinary(const std::string& filePath);
  std::string makeSignature(const SignatureData& data);
  std::string generateTimeStamp();
  std::string generateNonce();
  virtual void addCustomHeader(CHttpPost& post);

  std::string getUploadUrl() const { return "api.twitpic.com"; }
  std::string getUploadPath() const { return "/1/upload.xml"; }

protected:
  std::string nonce;
  std::string timeStamp;

  std::list<std::string> filelist_;
  std::string apiKey_;
  std::string tweetMsg_;

  CTwitterOAuth& OAuth;
};

