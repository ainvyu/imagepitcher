// imagepitcher.cpp : main source file for imagepitcher.exe
//

#include "stdafx.h"

#include "resource.h"

#include "pindlg.h"
#include "optiondlg.h"
#include "progressdlg.h"

#include "oauth.h"
#include "twitpicuploader.h"

using namespace std;
namespace po = boost::program_options;
CAppModule _Module;

bool DoUpload(CTwitterOAuth& OAuth, CTwitpicUploader& Uploader)
{
  std::string accessToken = CONVERT_MULTIBYTE(
    CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessToken")));

  if (accessToken.empty()) {
    // Start Authorize
    Uploader.doAuthorize();

    CPinDlg pinDlg;
    pinDlg.DoModal();

    const string& pin = pinDlg.getPin();
    OAuth.setOAuthVerifier(pin);
    OAuth.doAccessToken();

    // Save OAuth data.
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("RequestToken"),
      CONVERT_WIDE(OAuth.getRequestToken()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("RequestTokenSecret"),
      CONVERT_WIDE(OAuth.getRequestTokenSecret()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("AccessToken"),
      CONVERT_WIDE(OAuth.getAccessToken()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("AccessSecret"),
      CONVERT_WIDE(OAuth.getAccessSecret()));
  }
  else {
    string requestToken = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestToken")));
    string requestTokenSecret = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestTokenSecret")));
    string accessSecret = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessSecret")));

    OAuth.setRequestToken(requestToken);
    OAuth.setRequestTokenSecret(requestTokenSecret);
    OAuth.setAccessToken(accessToken);
    OAuth.setAccessSecret(accessSecret);
  }

  bool bRet;
  boost::thread UploadThread(bind(&CTwitpicUploader::doUpload, &Uploader));
  UploadThread.detach();
  CProgressDlg ProgressDlg;
  ProgressDlg.setUploader(&Uploader);
  ProgressDlg.DoModal(); // Block
  
  bRet = Uploader.isComplete(); 
  return bRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
  vector<tstring> argv;
  vector<tstring> files;
  po::variables_map vm;
  try {
    argv = po::split_winmain(lpstrCmdLine);
    po::options_description desc("Allowed options");
    po::positional_options_description p;
    // p.add("files", -1 )은 기본 옵션 -아무런 옵션 이름을 안주고 인자를 주면 
    // 그거에 대한 인자를 준걸로 받아들임
    p.add("files", -1);
    
    desc.add_options()
      ("option,o", "produce a help message")
      ("files,f", po::wvalue<vector<tstring>>(&files)->multitoken(), "files list")
      ;

    po::store(
      po::wcommand_line_parser(argv).options(desc).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (std::exception& e) {
    MessageBoxA(NULL, e.what(), "Error", MB_OK);
    return 0;
  }
  catch (...) {
    MessageBoxA(NULL, "Unknown error", "Error", MB_OK);
    return 0;
  } 

  HRESULT hRes = ::CoInitialize(NULL);
  // If you are running on NT 4.0 or higher you can use the following call instead to 
  // make the EXE free threaded. This means that calls come in on a random RPC thread.
  //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  ATLASSERT(SUCCEEDED(hRes));

  // Initialize String Encode class
  CStringEncode::InitLocale();

  // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
  ::DefWindowProc(NULL, 0, 0, 0L);

  AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  int nRet = 0;

  if (vm.count("option")) {
    COptionDlg dlg;
    dlg.DoModal();
  }
  else {
    CAppDataFile::GetInstance()->Init(_T("ImagePitcher"), CUtil::GetModulePath());

    CTwitterOAuth OAuth;
    std::string consumerKey = "8fxwQe0zkggUtgeg9Cw6FQ";
    std::string consumerSecret = "H0r6ELPFd973ylH7e1IwQA8BVdGIsSOAZgwGX2J3SKo";

    OAuth.setConsumerKey(consumerKey);
    OAuth.setConsumerSecret(consumerSecret);
    OAuth.setSignatureMethod("HMAC-SHA1");

    CTwitpicUploader Uploader(OAuth);

    // 경로 추가
    foreach (it, files) {
      const tstring path(*it);
      Uploader.addPicture(CONVERT_UTF8(path));
    }

    std::string tweetMsg; // 일단 빈칸으로..
    Uploader.setAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
    Uploader.setTweetMessage(tweetMsg);

    if (DoUpload(OAuth, Uploader)) {
      // Copy to Clipboard
      const list<string>& urlList = Uploader.getReponseUrlList();
      tstring clipboardText = CONVERT_WIDE(CStringUtil::Join(urlList, " "));
      CClipboard::CopyTextToClipboard(clipboardText.c_str());

      MessageBox(NULL, _T("Upload Success"), _T("Upload Success"), MB_OK);
    }
    else {
      MessageBox(NULL, _T("Upload Fail"), _T("Upload Fail"), MB_OK);
    }
  }

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}