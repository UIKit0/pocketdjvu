#pragma once

#include "./SIPState.h"
#include "./TimingDlg.h"
#include "./VMDlg.h"

class COptionsDlg :
  public WTL::CPropertySheetImpl<COptionsDlg>
{
  typedef WTL::CPropertySheetImpl<COptionsDlg> Base;

public:
  COptionsDlg();

  void OnSheetInitialized();

BEGIN_MSG_MAP(COptionsDlg)
  COMMAND_ID_HANDLER(IDOK, OnClickedOK)
  COMMAND_ID_HANDLER(IDCANCEL, OnClickedCancel)
  CHAIN_MSG_MAP(Base)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

// DATA:
private:
  CSIPState m_sip;
  CVMDlg      m_VMPg;
  CTimingDlg  m_TimingPg;
};
