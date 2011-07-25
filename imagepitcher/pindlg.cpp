// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "pindlg.h"

LRESULT CPinDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CenterWindow(GetParent());
  return TRUE;
}

LRESULT CPinDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(wID);
  return 0;
}

LRESULT CPinDlg::OnBnClickedButtonPin(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  char szPin[32] = {0};
  GetDlgItemTextA(this->m_hWnd, IDC_EDIT_PIN, szPin, 32);

  pin = szPin;

  EndDialog(wID);

  return 0;
}
