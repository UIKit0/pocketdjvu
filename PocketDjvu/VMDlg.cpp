#include "StdAfx.h"

#include "./VMDlg.h"
#include "./misc.h"

CVMDlg::CVMDlg() : m_storage( HKEY_CURRENT_USER, APP_REG_PATH_VM )
{
}

CVMDlg::~CVMDlg()
{
}

LRESULT CVMDlg::OnInitDialog( UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandled )
{
  m_storage.Load();
  DlgResize_Init( false, false, 0 );
  DoDataExchange();

  m_LevelSlider.SetRange( 0, 2 );
  m_LevelSlider.SetPos( m_storage.Level );

  m_spin.SetRange( g_cSwapLowLimitMB, g_cSwapUpperLimitMB );
  
  return 0;
}

int CVMDlg::OnApply()
{
#ifndef _WTL_NEW_PAGE_NOTIFY_HANDLERS
#   error It is necessary for right property pages handling.
#endif

  // PSNRET_NOERROR = apply OK
  // PSNRET_INVALID = apply not OK, return to this page
  // PSNRET_INVALID_NOCHANGEPAGE = apply not OK, don't change focus
  int ret = DoDataExchange(TRUE) ? PSNRET_NOERROR : PSNRET_INVALID;

  m_storage.Level = m_LevelSlider.GetPos();

  if ( PSNRET_NOERROR == ret )
  {
    m_storage.Save();
  }
  return ret;
}

LRESULT CVMDlg::OnBrowsePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  WTL::CFileDialog fd( TRUE, 0, 0, OFN_PROJECT|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY, 0, m_hWnd );
  if ( IDOK != fd.DoModal() )
  {
    return 0;
  }

  m_storage.SwapFileName = fd.m_szFileName;
  m_storage.SwapFileName += SWAP_NAME;

  DoDataExchange( FALSE, IDC_SWAPFILE );
  return 0;
}

void CVMDlg::OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data )
{
  BaseEx::OnDataValidateError( nCtrlID, bSave, data );
  NotifyDataValidateError<CVMDlg>( m_hWnd, nCtrlID, bSave, data );
}
