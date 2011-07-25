// imagepitcher.cpp : main source file for imagepitcher.exe
//

#include "stdafx.h"

#include "resource.h"

#include "pindlg.h"
#include "optiondlg.h"

#include "oauth.h"
#include "twitpicuploader.h"

using namespace std;

CAppModule _Module;

void DoUpload(CTwitterOAuth& OAuth, CTwitpicUploader& Uploader)
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

  Uploader.doUpload();
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
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

  if (lpstrCmdLine == _T("/o")) {
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
    Uploader.addPicture(CONVERT_UTF8(lpstrCmdLine));

    std::string tweetMsg; // ÀÏ´Ü ºóÄ­À¸·Î..
    Uploader.setAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
    Uploader.setTweetMessage(tweetMsg);

    DoUpload(OAuth, Uploader);
  }

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}