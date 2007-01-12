#pragma once

#include "SIPState.h"

class CGoToPage :
  public CDialogImpl<CGoToPage>,
  public CWinDataExchange<CGoToPage>
{
public:
	enum { IDD = IDD_GOTO_PAGE };

  CGoToPage( int maxPg, int curPg ) : m_maxPg(maxPg), m_curPg(curPg)
  {
  }

  int GetPage()
  {
    return m_curPg;
  }

  BEGIN_DDX_MAP(CGoToPage)
    DDX_UINT_RANGE(IDC_PG_NUM, m_curPg, 1, m_maxPg)
  END_DDX_MAP()

	BEGIN_MSG_MAP(CGoToPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
  END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
  int m_maxPg;
  int m_curPg;
  CUpDownCtrl m_UpDn;
  CSIPState m_SIPstate;
};
