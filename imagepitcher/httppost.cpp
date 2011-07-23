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

bool CHttpPost::doPost(void) {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(url_, port_);

  tcp::resolver::iterator endpoint_iterator;
  try {
    endpoint_iterator = resolver.resolve(query);
  }
  catch (std::exception& /*ex*/) {
    return false;
  }

  tcp::resolver::iterator end;

  tcp::socket socket(io_service_);
  boost::system::error_code error = boost::asio::error::host_not_found;

  while (error && endpoint_iterator != end)
  {
    socket.close();
    socket.connect(*endpoint_iterator++, error);
  }

  if (!!error) {
    return false;
  }

  stringstream post;
  post << "POST " << path_ << ' ' << "HTTP/1.1" << "\r\n";
  post << "Host: " << url_ << "\r\n";
  post << "Connection: keep-alive" << "\r\n";
  post << "Content-Length: " << content_.size() << "\r\n";
  post << "User-Agent: " << userAgent_ << "\r\n";
  post << "Content-Type: multipart/form-data;";
  if (!boundary_.empty())
    post << " boundary=" << boundary_ << "\r\n";
  else
    post << "\r\n";

  for (auto it = customHeader_.cbegin(), end = customHeader_.cend(); 
       it != end; ++it) {
    string section = (*it).first;
    const list<PostItem>& itemList = (*it).second;

    // 별도로 등록된 Section 구분자를 있는지 찾고 그에 맞는 값을 넣는다.
    auto sep_it = sectionSepHash_.find(section);
    if (sep_it == sectionSepHash_.end())
      post << section << ": ";
    else
      post << section << (*sep_it).second;
    
    std::string itemsHeaderString;
    itemsHeaderString.reserve(33);
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

  post << "\r\n";

  const string& postString = post.str();
  // Send POST Header
  socket.send(boost::asio::buffer(postString.c_str(), postString.size()));

  // Send POST Content
  int totalSendedSize = 0;
  int remainSize = content_.size();
  int streamSize = 0;
  try {
    while (remainSize != 0) {
      int sendSize;

      if (remainSize <= 16*1024)
        sendSize = remainSize;
      else
        sendSize = 16*1024;

      streamSize = socket.send(boost::asio::buffer(content_.c_str()+totalSendedSize, sendSize));
      remainSize -= streamSize;
      totalSendedSize += streamSize;
    }
  }
  catch(const std::exception& ex) {
    MessageBoxA(NULL, ex.what(), "Error", MB_OK);
    return false;
  }
  
  string response;
  response.reserve(1024);
  while (1)
  {
    char buf[1024] = {0};
    boost::system::error_code error;

    size_t len = socket.read_some(boost::asio::buffer(buf), error);

    if (error == boost::asio::error::eof)
      break; // Connection closed cleanly by peer.
    else if (error)
      return false;
      //throw boost::system::system_error(error); // Some other error.

    response += buf;
  }

  response_ = response;

  return true;
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