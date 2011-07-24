#include "StdAfx.h"
#include "resource.h"

#include "MainDlg.h"
#include "AboutDlg.h"
#include "appdatafile.h"

using namespace std;

CMainDlg::CMainDlg()
  : Uploader(OAuth)
{
  CAppDataFile::GetInstance()->Init(_T("ImagePitcher"), CUtil::GetModulePath());

  std::string consumerKey = "8fxwQe0zkggUtgeg9Cw6FQ";
  std::string consumerSecret = "H0r6ELPFd973ylH7e1IwQA8BVdGIsSOAZgwGX2J3SKo";

  OAuth.setConsumerKey(consumerKey);
  OAuth.setConsumerSecret(consumerSecret);
  OAuth.setSignatureMethod("HMAC-SHA1");
}

BOOL CMainDlg::PreTranslateMessage( MSG* pMsg )
{
  return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
  return FALSE;
}

LRESULT CMainDlg::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
  // center the dialog on the screen
  CenterWindow();

  // set icons
  HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
  SetIcon(hIcon, TRUE);
  HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
  SetIcon(hIconSmall, FALSE);

  // register object for message filtering and idle updates
  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->AddMessageFilter(this);
  pLoop->AddIdleHandler(this);

  UIAddChildWindowContainer(m_hWnd);

  return TRUE;
}

LRESULT CMainDlg::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
  // unregister message filtering and idle updates
  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->RemoveMessageFilter(this);
  pLoop->RemoveIdleHandler(this);

  return 0;
}

LRESULT CMainDlg::OnAppAbout( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
  CAboutDlg dlg;
  dlg.DoModal();
  return 0;
}

LRESULT CMainDlg::OnOK( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
  // TODO: Add validation code 
  //CloseDialog(wID);
  Uploader.doAuthorize();

  return 0;
}

LRESULT CMainDlg::OnCancel(
  WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
  CloseDialog(wID);
  return 0;
}

void CMainDlg::CloseDialog( int nVal )
{
  DestroyWindow();
  ::PostQuitMessage(nVal);
}

// LRESULT CMainDlg::OnBnClickedButtonPin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// {
//   char pin[32] = {0};
//   GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PIN, pin, 32);
// 
//   OAuth.setOAuthVerifier(pin);
//   OAuth.doAccessToken();
// 
//   Uploader.addPicture("G:\\test.jpg");
//   Uploader.setAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
//   Uploader.setTweetMessage("test");
//   Uploader.doUpload();
// 
//   return 0;
// }

LRESULT CMainDlg::OnBnClickedButtonPin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  char pin[32] = {0};
  GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PIN, pin, 32);

  std::string accessToken = CONVERT_MULTIBYTE(
    CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessToken")));

  if (accessToken.empty()) {
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
    std::string requestToken = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestToken")));
    std::string requestTokenSecret = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestTokenSecret")));
    std::string accessSecret = CONVERT_MULTIBYTE(
      CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessSecret")));
    
    OAuth.setRequestToken(requestToken);
    OAuth.setRequestTokenSecret(requestTokenSecret);
    OAuth.setAccessToken(accessToken);
    OAuth.setAccessSecret(accessSecret);
  }

  string tweetMsg; // ÀÏ´Ü ºóÄ­À¸·Î..

  Uploader.addPicture("G:\\test.jpg");
  Uploader.setAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
  Uploader.setTweetMessage(tweetMsg);
  Uploader.doUpload();
  
  return 0;
}