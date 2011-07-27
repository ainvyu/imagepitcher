// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "progressdlg.h"
#include "twitpicuploader.h"

#define ID_EVENT_REFRESH_CONNECTION_LIST 10005

BOOL CProgressDlg::OnIdle()
{
  return FALSE;
}

LRESULT CProgressDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CenterWindow(GetParent());

  progressCtrl = GetDlgItem(IDC_PROGRESS);
  progressCtrl.SetRange(0, 100);
  progressCtrl.SetPos(0);

  SetTimer(ID_EVENT_REFRESH_CONNECTION_LIST, 300);

  return TRUE;
}

LRESULT CProgressDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(wID);
  return 0;
}

void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
  switch (nIDEvent) {
  case ID_EVENT_REFRESH_CONNECTION_LIST:
    RefreshProgressBar();
    break;
  }
}

void CProgressDlg::RefreshProgressBar()
{
  if (pUploader) {
    int percent = pUploader->getProgressPercent();
    progressCtrl.SetPos(percent);

    

    if (pUploader->isFail() || pUploader->isComplete()) {
      KillTimer(ID_EVENT_REFRESH_CONNECTION_LIST);
      EndDialog(ID_EVENT_REFRESH_CONNECTION_LIST);
    }
  }
}

LRESULT CProgressDlg::OnUpdateProgressBar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
  return 0;
}