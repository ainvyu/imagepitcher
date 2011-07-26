// optiondlg.cpp : implementation of the COptionDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "optiondlg.h"

const TCHAR* SHELLEX_FILEPATH = _T("SimpleHFSShellEx.dll");
const TCHAR* strEleQuestion = _T("관리자 권한으로 실행해야 가능합니다.\n권한 상승을 하겠습니까?");

LRESULT COptionDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CenterWindow(GetParent());

  return TRUE;
}

LRESULT COptionDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(wID);

  return 0;
}


LRESULT COptionDlg::OnBnClickedButtonShellExRegister(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  BOOL bElevated;
  if (SUCCEEDED(CUtil::IsElevated(&bElevated)))
  {
    if (!bElevated)
    {
      if (IDYES == MessageBox(strEleQuestion, _T("Error"), MB_YESNO | MB_ICONQUESTION))
      {
        CUtil::ShellExecWithElevation(NULL, CUtil::GetModulePathName().c_str(), NULL, NULL);
        GetParent().PostMessage(WM_CLOSE);
      }

      return 0;
    }
  }

  tstringstream ssCmd;
  ssCmd << _T("\"") << CUtil::GetModulePathName() << _T("\"") << _T(" ")
    << _T("\"") << _T("%1") << _T("\"");

//   if (CRegisterMenu::Register(_T("Image Pitcher"), _T(".jpg"), _T("Upload to Twitpic"), _T("Upload to Twitpic"), ssCmd.str(), _T("JPEG")) &&
//       CRegisterMenu::Register(_T("Image Pitcher"), _T(".gif"), _T("Upload to Twitpic"), _T("Upload to Twitpic"), ssCmd.str(), _T("GIF")) &&
//       CRegisterMenu::Register(_T("Image Pitcher"), _T(".png"), _T("Upload to Twitpic"), _T("Upload to Twitpic"), ssCmd.str(), _T("PNG")))
  if (CRegisterMenu::Register(_T("Image Pitcher"), _T("*"), _T("Upload to Twitpic"), _T("Upload to Twitpic"), ssCmd.str(), _T("All extension")))
    MessageBox(_T("Register Success"), _T("Success!"), MB_OK);
  else
    MessageBox(CUtil::GetLastErrorMessage().c_str());

// 쉘 익스텐션 등록.
//	if (CRegSvr32::Register(SHELLEX_FILEPATH))
//	{
//		MessageBox(_T("Register Success"), _T("Success!"), MB_OK);
// 	}
// 	else
// 	{
// 		MessageBox(_T("Register Fail"), _T("Fail!"), MB_OK | MB_ICONERROR);
// 	}

  return 0;
}


LRESULT COptionDlg::OnBnClickedButtonShellExUnregister(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  BOOL bElevated;
  if (SUCCEEDED(CUtil::IsElevated(&bElevated)))
  {
    if (!bElevated)
    {
      if (IDYES == MessageBox(strEleQuestion, _T("Error"), MB_YESNO | MB_ICONQUESTION))
      {
        CUtil::ShellExecWithElevation(NULL, CUtil::GetModulePathName().c_str(), NULL, NULL);
        GetParent().PostMessage(WM_CLOSE);
      }

      return 0;
    }
  }

//   if (CRegisterMenu::UnRegister(_T("Image Pitcher"), _T(".jpg"), _T("Upload to Twitpic")) &&
//       CRegisterMenu::UnRegister(_T("Image Pitcher"), _T(".gif"), _T("Upload to Twitpic")) &&
//       CRegisterMenu::UnRegister(_T("Image Pitcher"), _T(".png"), _T("Upload to Twitpic")))
  if (CRegisterMenu::UnRegister(_T("Image Pitcher"), _T("*"), _T("Upload to Twitpic")))
    MessageBox(_T("Unregister Success"), _T("Success!"), MB_OK);
  else
    MessageBox(CUtil::GetLastErrorMessage().c_str());

// 	if (CRegSvr32::UnRegister(SHELLEX_FILEPATH))
// 	{
// 		MessageBox(_T("Unregister Success"), _T("Success!"), MB_OK);
// 	}
// 	else
// 	{
// 		MessageBox(_T("Unregister Fail"), _T("Fail!"), MB_OK | MB_ICONERROR);
// 	}

  return 0;
}