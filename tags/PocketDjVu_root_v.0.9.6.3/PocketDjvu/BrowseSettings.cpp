#include "stdafx.h"

#include "./BrowseSettings.h"
#include "./misc.h"

CBrowseSettings::CBrowseSettings() :
  m_storage( HKEY_CURRENT_USER, APP_REG_PATH_BROWSE )
{
}

CBrowseSettings::~CBrowseSettings()
{
}

LRESULT CBrowseSettings::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  m_storage.Load();
  DlgResize_Init( false, false, 0 );
  DoDataExchange();

  {
    WTL::CString s;
    s.LoadString( IDS_BR_DEF_MODE );
    m_comboMode.AddString( !s.IsEmpty() ? s : L"Default mode" );

    s.Empty();
    s.LoadString( IDS_BR_PARROT_MODE );
    m_comboMode.AddString( !s.IsEmpty() ? s : L"Parrot mode" );
    m_comboMode.SetCurSel( m_storage.browseMode );  
  }

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
    m_storage.browseMode = m_comboMode.GetCurSel();
    m_storage.Save();
    CValues::Assign( m_storage );
  }
  return ret;
}
