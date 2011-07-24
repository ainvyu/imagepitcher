// imagepitcher.cpp : main source file for imagepitcher.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  CTwitterOAuth OAuth;
  std::string consumerKey = "8fxwQe0zkggUtgeg9Cw6FQ";
  std::string consumerSecret = "H0r6ELPFd973ylH7e1IwQA8BVdGIsSOAZgwGX2J3SKo";

  OAuth.setConsumerKey(consumerKey);
  OAuth.setConsumerSecret(consumerSecret);
  OAuth.setSignatureMethod("HMAC-SHA1");
  
  CTwitpicUploader Uploader(OAuth);
  Uploader.addPicture(CONVERT_MULTIBYTE(lpstrCmdLine));

  std::string tweetMsg; // ÀÏ´Ü ºóÄ­À¸·Î..
  Uploader.setAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
  Uploader.setTweetMessage(tweetMsg);

  CMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  CMainDlg dlgMain(OAuth, Uploader);

  if(dlgMain.Create(NULL) == NULL)
  {
    ATLTRACE(_T("Main dialog creation failed!\n"));
    return 0;
  }

  dlgMain.ShowWindow(nCmdShow);

  int nRet = theLoop.Run();

  _Module.RemoveMessageLoop();
  return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
  HRESULT hRes = ::CoInitialize(NULL);
  CStringEncode::InitLocale();
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  ATLASSERT(SUCCEEDED(hRes));

  // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
  ::DefWindowProc(NULL, 0, 0, 0L);

  AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  int nRet = Run(lpstrCmdLine, nCmdShow);

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}
