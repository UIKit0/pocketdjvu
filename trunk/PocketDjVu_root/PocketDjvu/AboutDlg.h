// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAboutDlg : public CStdDialogResizeImpl<CAboutDlg>
{
  typedef CStdDialogResizeImpl<CAboutDlg> Base;
public:
  enum { IDD = IDD_ABOUTBOX };

  BEGIN_DLGRESIZE_MAP(CAboutDlg)
    BEGIN_DLGRESIZE_GROUP()
      DLGRESIZE_CONTROL(IDC_STATIC_CR, DLSZ_SIZE_X)
      DLGRESIZE_CONTROL(IDC_STATIC_LIC, DLSZ_SIZE_X)
    END_DLGRESIZE_GROUP()
  END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CAboutDlg)
    CHAIN_MSG_MAP(Base)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
