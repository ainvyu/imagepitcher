#include "StdAfx.h"
#include "resource.h"

#include "MainDlg.h"
#include "AboutDlg.h"
#include "appdatafile.h"

CMainDlg::CMainDlg()
  : uploader_(oauth_)
{
  CAppDataFile::GetInstance()->Init(_T("ImagePitcher"), CUtil::GetModulePath());
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
  uploader_.DoAuthorize();

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

LRESULT CMainDlg::OnBnClickedButtonPin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  char pin[32] = {0};
  GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PIN, pin, 32);

  std::string accessToken = CONVERT_MULTIBYTE(
    CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessToken")));

//  if (accessToken.empty()) {
    oauth_.SetOAuthVerifier(pin);
    oauth_.DoAccessToken();

    // Save OAuth data.
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("RequestToken"),
      CONVERT_WIDE(oauth_.GetRequestToken()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("RequestTokenSecret"),
      CONVERT_WIDE(oauth_.GetRequestTokenSecret()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("AccessToken"),
      CONVERT_WIDE(oauth_.GetAccessToken()));
    CAppDataFile::GetInstance()->SetAppData(_T("OAuth"), _T("AccessSecret"),
      CONVERT_WIDE(oauth_.GetAccessSecret()));
//  }
//   else {
//     std::string requestToken = CONVERT_MULTIBYTE(
//       CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestToken")));
//     std::string requestTokenSecret = CONVERT_MULTIBYTE(
//       CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("RequestTokenSecret")));
//     std::string accessSecret = CONVERT_MULTIBYTE(
//       CAppDataFile::GetInstance()->GetAppData(_T("OAuth"), _T("AccessSecret")));
//     
//     oauth_.SetRequestToken(requestToken);
//     oauth_.SetRequestTokenSecret(requestTokenSecret);
//     oauth_.SetAccessToken(accessToken);
//     oauth_.SetAccessSecret(accessSecret);
//   }

  uploader_.AddPicture("G:\\test.jpg");
  uploader_.SetAPIKey("f0f31e3f13e8f1dfed50ab4e22a27b60");
  uploader_.SetTweetMessage("test");
  uploader_.DoUpload();
  
  return 0;
}