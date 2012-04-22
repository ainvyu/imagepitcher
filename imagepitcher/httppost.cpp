#include "StdAfx.h"
#include "httppost.h"

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

const int MTU_SIZE = 1500;

CHttpPost::CHttpPost(void)
  : port_("80")
  , postProgPercent(0)
  , responseProgPercent(0)
  , progCallbackFunc_([this] (const int& percent) {})
{
}


CHttpPost::~CHttpPost(void)
{
}

string CHttpPost::makeHeaderString() const {
  stringstream post;
  post << "POST " << path_ << ' ' << "HTTP/1.1" << "\r\n";
  post << "Host: " << url_ << "\r\n";
  post << "Connection: keep-alive" << "\r\n";
  // Locale 변경으로 인한 영향으로 걍 정수값 출력하면 comma이 포함됨 그래서 string으로
  post << "Content-Length: " << 
    CStringUtil::IntToStrA(content_.size()) << "\r\n";
  post << "User-Agent: " << userAgent_ << "\r\n";
  post << "Content-Type: multipart/form-data;";
  if (!boundary_.empty())
    post << " boundary=" << boundary_;

  post << "\r\n";

  return post.str();
}

string CHttpPost::makeCustomHeaderString() const {
  stringstream post;

  for (auto it = customHeader_.cbegin(), _end = customHeader_.cend(); 
    it != _end; ++it) {
      string section = (*it).first;
      const list<PostItem>& itemList = (*it).second;

      // 별도로 등록된 Section 구분자를 있는지 찾고 그에 맞는 값을 넣는다.
      auto sep_it = sectionSepHash_.find(section);
      if (sep_it == sectionSepHash_.end())
        post << section << ": ";
      else
        post << section << (*sep_it).second;

      string itemsHeaderString;
      itemsHeaderString.reserve(257);
      for (auto list_it = itemList.cbegin(), list_end = itemList.end();
        list_it != list_end; ++list_it) {
          const PostItem& item = (*list_it);

          if (!item.name.empty())
            itemsHeaderString += item.name + item.separator + "\"" + item.value + "\", ";
          else
            itemsHeaderString += item.value + "; ";
      }
      if (!itemsHeaderString.empty()) {
        // 맨 뒤에 공백 제거
        itemsHeaderString.erase(itemsHeaderString.end()-1);
        // 그리고 맨 마지막의 ,나 ; 제거
        itemsHeaderString.erase(itemsHeaderString.end()-1);
      }

      post << itemsHeaderString;
      post << "\r\n";
  }

  return post.str();
}

bool CHttpPost::sendContent(tcp::socket& socket) {

  // Send POST Content
  int totalSendedSize = 0;
  int remainSize = content_.size();
  int streamSize = 0;

  boost::system::error_code errCode;
  while (remainSize != 0) {
    int sendSize;

    if (remainSize <= 16*1024)
      sendSize = remainSize;
    else
      sendSize = 16*1024;

    string::const_pointer pNextPacket = content_.c_str()+totalSendedSize;
    streamSize = socket.send(
      boost::asio::buffer(pNextPacket, sendSize), 0, errCode);

    if (errCode)
      return false;
    
    remainSize -= streamSize;
    totalSendedSize += streamSize;

    if (totalSendedSize != 0) {
      int percent = (totalSendedSize / content_.size() * 100);
      setPostPercent(percent);
    }
  }

  return true;
}

bool CHttpPost::doPost(void) {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(url_, port_);

  boost::system::error_code errCode;
  tcp::resolver::iterator endpoint_iterator;
  endpoint_iterator = resolver.resolve(query, errCode);
  if (errCode)
    return false;

  tcp::resolver::iterator _end;

  boost::system::error_code ec;
  tcp::socket socket(io_service_);

  boost::system::error_code error = boost::asio::error::host_not_found;

  while (error && endpoint_iterator != _end) {
    socket.close();
    socket.connect(*endpoint_iterator++, error);
  }

  if (error) {
    return false;
  }

  tcp::no_delay option(true);
  socket.set_option(option, ec);
  if (ec) {
    string ecMsg = ec.message();
  }

  stringstream post;
  post << makeHeaderString();
  post << makeCustomHeaderString();
  post << "\r\n";

  const string& postString = post.str();
  // Send POST Header
  socket.send(boost::asio::buffer(postString.c_str(), postString.size()));

  if (!sendContent(socket))
    return false;
  
  // 안하면 상대방이 response 다 보내도 않끊긴다.
  socket.shutdown(tcp::socket::shutdown_send);

  try {
    response_ = recvResponse(socket);
  }
  catch (boost::system::system_error& /*err*/) {
    return false;
  }

  if (response_.empty())
    return false;

  return true;
}

std::string CHttpPost::recvResponse(tcp::socket& socket) {
  char buf[MTU_SIZE] = {0};
  string response;
  response.reserve(2049);
  while (1) {
    boost::system::error_code error;

    size_t len = socket.read_some(boost::asio::buffer(buf, MTU_SIZE), error);

    if (error == boost::asio::error::eof)
      break; // Connection closed cleanly by peer.
    else if (error)
      throw boost::system::system_error(error); // Some other error. WTH?

    response.append(&buf[0], &buf[len]);
  }

  setResponsePercent(100);

  return response;
}

void CHttpPost::addHeader( const std::string& _section, PostItem _item )
{
  auto it = customHeader_.find(_section);
  if (it == customHeader_.end()) {
    pair<headerHash::iterator, bool> item = 
      customHeader_.insert(make_pair(_section, list<PostItem>()));
    it = item.first;
  }
  
  list<PostItem>& itemList = (*it).second;
  itemList.push_back(_item);
}

void CHttpPost::registerSectionSeparator( 
  const std::string& _section, const std::string& _sep )
{
  sectionSepHash_.insert(make_pair(_section, _sep));
}