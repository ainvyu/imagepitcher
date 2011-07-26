#include "StdAfx.h"
#include "httppost.h"

using namespace std;
namespace fs = boost::filesystem;
using boost::asio::ip::tcp;

CHttpPost::CHttpPost(void)
  : port_("80")
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
  // Locale �������� ���� �������� �� ������ ����ϸ� comma�� ���Ե� �׷��� string����
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

      // ������ ��ϵ� Section �����ڸ� �ִ��� ã�� �׿� �´� ���� �ִ´�.
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
        // �� �ڿ� ���� ����
        itemsHeaderString.erase(itemsHeaderString.end()-1);
        // �׸��� �� �������� ,�� ; ����
        itemsHeaderString.erase(itemsHeaderString.end()-1);
      }

      post << itemsHeaderString;
      post << "\r\n";
  }

  return post.str();
}

bool CHttpPost::sendContent(tcp::socket& socket) const {

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

  tcp::socket socket(io_service_);
  boost::system::error_code error = boost::asio::error::host_not_found;

  while (error && endpoint_iterator != _end)
  {
    socket.close();
    socket.connect(*endpoint_iterator++, error);
  }

  if (error) {
    return false;
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
  
  response_ = recvResponse(socket);
  if (response_.empty())
    return false;

  return true;
}

std::string CHttpPost::recvResponse(tcp::socket& socket) const {
  char buf[1500] = {0};
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
  }

  return response;
}

void CHttpPost::addHeader( const std::string& _section, PostItem _item )
{
  auto it = customHeader_.find(_section);
  if (it == customHeader_.end()) {
    pair<headerHash::iterator, bool> item = 
      customHeader_.insert(headerHash::value_type(_section, list<PostItem>()));
    it = item.first;
  }
  
  list<PostItem>& itemList = (*it).second;
  itemList.push_back(_item);
}

void CHttpPost::registerSectionSeparator( const std::string& _section, const std::string& _sep )
{
  sectionSepHash_.insert(unordered_map<string, string>::value_type(_section, _sep));
}