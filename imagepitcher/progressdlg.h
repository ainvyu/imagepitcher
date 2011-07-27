// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WM_UPDATE_PROGRESSBAR 30871

class CTwitpicUploader;

class CProgressDlg : public CDialogImpl<CProgressDlg>
{
public:
  enum { IDD = IDD_PROGRESSDLG };

  BEGIN_MSG_MAP(CProgressDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    MESSAGE_HANDLER(WM_UPDATE_PROGRESSBAR, OnUpdateProgressBar)
    MSG_WM_TIMER(OnTimer)
  END_MSG_MAP()

  virtual BOOL OnIdle();

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  LRESULT OnUpdateProgressBar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  void RefreshProgressBar();
  void OnTimer(UINT_PTR nIDEvent);

  void setUploader(CTwitpicUploader* val) { pUploader = val; }

private:
  CProgressBarCtrl progressCtrl;
  CTwitpicUploader* pUploader;
};