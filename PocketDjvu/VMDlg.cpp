#include "StdAfx.h"

#include "./VMDlg.h"
#include "./misc.h"

CVMDlg::CVMDlg() : m_regVmValues( HKEY_CURRENT_USER, APP_REG_PATH_VM )
{
}

CVMDlg::~CVMDlg()
{
}

LRESULT CVMDlg::OnInitDialog( UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandled )
{
    m_regVmValues.Load();
    DlgResize_Init( false, false, 0 );
    DoDataExchange();

    m_LevelSlider.SetRange( 0, 2 );
    m_LevelSlider.SetPos( m_regVmValues.Level );

    m_spin.SetRange( g_cSwapLowLimitMB, g_cSwapUpperLimitMB );
    
    m_ramPercent.SetRange( g_cVmRAMLowPercentage, g_cVmRAMHighPercentage );
    m_ramPercent.SetPos( m_regVmValues.RamPercent );

    ToggleControlsByVmMode();

    return 0;
}

void CVMDlg::ToggleControlsByVmMode()
{
    bool bEnable = false;
    if ( 0 == m_regVmValues.SwapOrRam )
    {
        bEnable = true;
    }
    //........................................
    GetDlgItem(IDC_SWAPFILE).EnableWindow( bEnable );
    GetDlgItem(IDC_BROWSE_PATH).EnableWindow( bEnable );
    GetDlgItem(IDC_MBSIZE).EnableWindow( bEnable );
    m_spin.EnableWindow( bEnable );
    //........................................
    GetDlgItem(IDC_PERCENT).EnableWindow( !bEnable );
    m_ramPercent.EnableWindow( !bEnable );
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
  if ( PSNRET_NOERROR == ret )
  {
      m_regVmValues.Level      = m_LevelSlider.GetPos();
      m_regVmValues.RamPercent = m_ramPercent.GetPos();

      m_regVmValues.Save();
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

  m_regVmValues.SwapFileName = fd.m_szFileName;
  m_regVmValues.SwapFileName += SWAP_NAME;

  DoDataExchange( FALSE, IDC_SWAPFILE );
  return 0;
}

void CVMDlg::OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data )
{
  BaseEx::OnDataValidateError( nCtrlID, bSave, data );
  NotifyDataValidateError<CVMDlg>( m_hWnd, nCtrlID, bSave, data );
}

LRESULT CVMDlg::OnBnClickedSwapOrRAM(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    switch ( wID )
    {
    case IDC_SWAP_FILE:
        m_regVmValues.SwapOrRam = 0;
        ToggleControlsByVmMode();
    break;
    case IDC_RAM_ONLY:
        m_regVmValues.SwapOrRam = 1;
        ToggleControlsByVmMode();
    break;
    }

    return 0;
}
