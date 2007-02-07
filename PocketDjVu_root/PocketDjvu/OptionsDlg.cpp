#include "StdAfx.h"

#include "./OptionsDlg.h"
#include "./misc.h"

COptionsDlg::COptionsDlg()
{
  BOOL  res =  AddPage( m_VMPg );
  res = res && AddPage( m_TimingPg );
}

void COptionsDlg::OnSheetInitialized()
{
  ::CreateDlgMenuBar( IDR_MENU_OKCANCEL, m_hWnd );
}

LRESULT COptionsDlg::OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  bHandled = false; // NB!
  return 0;
}

LRESULT COptionsDlg::OnClickedCancel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  bHandled = false; // NB!
  return 0;
}
