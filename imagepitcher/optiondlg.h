// optiondlg.h : interface of the COptionDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class COptionDlg : public CDialogImpl<COptionDlg>
{
public:
  enum { IDD = IDD_OPTIONDLG };

  BEGIN_MSG_MAP(COptionDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
    COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    COMMAND_HANDLER(IDC_BUTTON_SHELLEX_REGISTER, BN_CLICKED, OnBnClickedButtonShellExRegister)
    COMMAND_HANDLER(IDC_BUTTON_SHELLEX_UNREGISTER, BN_CLICKED, OnBnClickedButtonShellExUnregister)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedButtonShellExRegister(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedButtonShellExUnregister(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};