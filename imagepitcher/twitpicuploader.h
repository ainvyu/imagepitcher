#pragma once

#include "oauth.h"

class CTwitterOAuth;

class CTwitpicUploader
{
public:
  CTwitpicUploader(CTwitterOAuth& _oauth);
  ~CTwitpicUploader(void);

  void SetAPIKey(const std::string& _apiKey) { apiKey_ = _apiKey; }
  void SetTweetMessage(const std::string& _twtMsg) { tweetMsg_ = _twtMsg; }

  void AddPicture(const std::string& strPath);
  void DoUpload();
  void DoAuthorize();

private:
  std::string MakeContent(const std::string& filePath, const std::string& boundary);
  std::string MakeBoundary();
  std::string GetImageContentType(const std::string& path);
  const std::vector<char> GetPictureBinary(const std::string& filePath);
  std::string MakeSignature(const SignatureData& data);
  std::string GenerateTimeStamp();
  std::string GenerateNonce();

private:
  std::list<std::string> filelist_;
  std::string apiKey_;
  std::string tweetMsg_;

  CTwitterOAuth& oauth_;
};

