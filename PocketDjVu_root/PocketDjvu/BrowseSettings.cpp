#include "stdafx.h"

#include "./BrowseSettings.h"
#include "./misc.h"

CBrowseSettings::CBrowseSettings() :
  m_storage( HKEY_CURRENT_USER, APP_REG_PATH_BROWSE )
{
  if ( ERROR_SUCCESS != m_storage.Load() )
  {
    m_storage.m_browseMode = CRegStorage::DEF_MODE;
    m_storage.m_pageScrollVertPercent = g_cPageScrollVertPercent;
    m_storage.m_pageScrollHorPercent  = g_cPageScrollHorPercent;
  }
}

CBrowseSettings::~CBrowseSettings()
{
}

LRESULT CBrowseSettings::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{  
  DlgResize_Init( false, false, 0 );
  DoDataExchange();

  m_comboMode.AddString( L"Default mode" );
  m_comboMode.AddString( L"Parrot mode" );
  m_comboMode.SetCurSel( m_storage.m_browseMode );

  m_spinVert.SetRange( 5, 100 );
  m_spinHor.SetRange( 5, 100 );

  return 0;
}

void CBrowseSettings::OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data )
{
  BaseEx::OnDataValidateError( nCtrlID, bSave, data );
  NotifyDataValidateError<CBrowseSettings>( m_hWnd, nCtrlID, bSave, data );
}

#ifndef _WTL_NEW_PAGE_NOTIFY_HANDLERS
#   error It is necessary for right property pages handling.
#endif
int CBrowseSettings::OnApply()
{
  // PSNRET_NOERROR = apply OK
  // PSNRET_INVALID = apply not OK, return to this page
  // PSNRET_INVALID_NOCHANGEPAGE = apply not OK, don't change focus
  int ret = DoDataExchange(TRUE) ? PSNRET_NOERROR : PSNRET_INVALID;

  if ( PSNRET_NOERROR == ret )
  {
    m_storage.Save();
  }
  return ret;
}
