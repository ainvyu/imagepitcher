#pragma once

#include "oauth.h"
#include "twitpicuploader.h"

// API v2

class CTwitpicUploader2: public CTwitpicUploader
{
public:
  CTwitpicUploader2(CTwitterOAuth& _oauth);
  virtual ~CTwitpicUploader2(void);

protected:
  virtual std::string makeContent(const std::string& filePath, 
                                  const std::string& boundary);
  virtual void addCustomHeaderTo(CHttpPost& post);

  std::string getUploadPath() const { return "/2/upload.xml"; }
};

