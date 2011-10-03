#pragma once
#include "httppost.h"
class TwitpicPost: public CHttpPost
{
public:
  TwitpicPost(void);
  virtual ~TwitpicPost(void);

  virtual std::string recvResponse(boost::asio::ip::tcp::socket& socket) const;
};

