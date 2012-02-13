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
  virtual bool doUpload();

  void doAuthorize();

  const std::list<std::string>& getReponseUrlList() const 
  { return reponseUrlList; }

  void setProgressPercent(int percent) { progressPercent = percent; }
  int getProgressPercent() const { return progressPercent; }
  bool isComplete() const { return completeFlag; }
  bool isFail() const { return failFlag; }

protected:
  virtual std::string makeContent(const std::string& filePath, 
                                  const std::string& boundary);
  std::string makeBoundary();
  std::string getImageContentType(const std::string& path);
  const std::vector<char> getPictureBinary(const std::string& filePath);
  std::string makeSignature(const SignatureData& data);
  std::string generateTimeStamp();
  std::string generateNonce();
  virtual void addCustomHeaderTo(CHttpPost& post);

  std::string getUploadUrl() const { return "api.twitpic.com"; }
  std::string getUploadPath() const { return "/1/upload.xml"; }

  bool parseResponse(const std::string response);

protected:
  int progressPercent;
  bool completeFlag;
  bool failFlag;

  std::string nonce;
  std::string timeStamp;

  std::list<std::string> fileList;
  std::string apiKey_;
  std::string tweetMsg_;

  CTwitterOAuth& OAuth;

  std::list<std::string> reponseUrlList;
};

