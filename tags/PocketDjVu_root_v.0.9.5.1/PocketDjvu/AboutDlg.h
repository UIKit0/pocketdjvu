#pragma once

#include "resource.h"
#include "SIPState.h"

class CAboutDlg : public WTL::CStdDialogResizeImpl<CAboutDlg>
{
  typedef WTL::CStdDialogResizeImpl<CAboutDlg> Base;
public:
  enum { IDD = IDD_ABOUTBOX };

  BEGIN_DLGRESIZE_MAP(CAboutDlg)
    DLGRESIZE_CONTROL(IDC_STATIC_VER, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_STATIC_CR,  DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_HOMEPAGE,   DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_STATIC_LIC, DLSZ_SIZE_X )
    DLGRESIZE_CONTROL(IDC_INFO, DLSZ_SIZE_X|DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CAboutDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    MESSAGE_HANDLER(WM_WININICHANGE, OnWininiChange)
    CHAIN_MSG_MAP(Base)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
public:
  LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:
  CSIPState m_sip;
  WTL::CHyperLink m_homePageLink;
};
