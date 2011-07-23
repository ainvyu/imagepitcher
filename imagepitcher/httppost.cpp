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

bool CHttpPost::DoPost(void) {
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

    // ������ ��ϵ� Section �����ڸ� �ִ��� ã�� �׿� �´� ���� �ִ´�.
    auto sep_it = sectionSepHash_.find(section);
    if (sep_it == sectionSepHash_.end())
      post << section << ": ";
    else
      post << section << (*sep_it).second;
    
    std::string items_string;
    items_string.reserve(33);
    for (auto list_it = itemList.cbegin(), list_end = itemList.end();
         list_it != list_end; ++list_it) {
      const PostItem& item = (*list_it);

      if (!item.name.empty())
        items_string += item.name + item.separator + "\"" + item.value + "\", ";
      else
        items_string += item.value + "; ";
    }
    if (!items_string.empty()) {
      // �� �ڿ� ���� ����
      items_string.erase(items_string.end()-1);
      // �׸��� �� �������� ,�� ; ����
      items_string.erase(items_string.end()-1);
    }

    post << items_string;
    post << "\r\n";
  }

  post << "\r\n";

  const string& strPost = post.str();
  // Send POST Header
  socket.send(boost::asio::buffer(strPost.c_str(), strPost.size()));

  // Send POST Content
  int totalSendedSize = 0;
  int remainSize = content_.size();

  try {
    while (remainSize != 0) {
      int sendSize;

      if (remainSize <= 16*1024)
        sendSize = remainSize;
      else
        sendSize = 16*1024;

      int streamSize = socket.send(boost::asio::buffer(content_.c_str()+totalSendedSize, sendSize));
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

void CHttpPost::AddHeader( const std::string& _section, PostItem _item )
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

void CHttpPost::RegisterSectionSeparator( const std::string& _section, const std::string& _sep )
{
  sectionSepHash_.insert(unordered_map<string, string>::value_type(_section, _sep));
}