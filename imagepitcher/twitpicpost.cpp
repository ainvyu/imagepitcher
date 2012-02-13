#include "StdAfx.h"
#include "twitpicpost.h"

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

TwitpicPost::TwitpicPost(void)
{
}


TwitpicPost::~TwitpicPost(void)
{
}

std::string TwitpicPost::recvResponse(tcp::socket& socket) {
  MessageBox(NULL, _T("test"), _T("test"), MB_OK);
  char buf[1501] = {0};
  string response;
  response.reserve(2049);
  while (1) {
    boost::system::error_code error;

    size_t len = socket.read_some(boost::asio::buffer(buf, 1500), error);

    if (error == boost::asio::error::eof)
      break; // Connection closed cleanly by peer.
    else if (error)
      return false;
    //throw boost::system::system_error(error); // Some other error.

    response.append(&buf[0], &buf[len]);

    if (response.rfind("</image>") != string::npos) {
      break;
    }
  }

  setResponsePercent(100);

  return response;
}