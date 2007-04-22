#pragma once

#include "./BrowseSettings.h"
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
  CHAIN_MSG_MAP(Base)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

// DATA:
private:
  CSIPState       m_sip;
  CBrowseSettings m_BrowsePg;
  CVMDlg          m_VMPg;
  CTimingDlg      m_TimingPg;
};
