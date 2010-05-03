#include "stdafx.h"

#include "./MiscSettings.h"
#include "./misc.h"

CMiscSettings::CMiscSettings()
    : m_storage( HKEY_CURRENT_USER, APP_REG_MISC )
{
}

CMiscSettings::~CMiscSettings()
{
}

LRESULT CMiscSettings::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  m_storage.Load();
  DlgResize_Init( false, false, 0 );
  DoDataExchange();

  return 0;
}

void CMiscSettings::OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data )
{
  BaseEx::OnDataValidateError( nCtrlID, bSave, data );
  NotifyDataValidateError<CMiscSettings>( m_hWnd, nCtrlID, bSave, data );
}

#ifndef _WTL_NEW_PAGE_NOTIFY_HANDLERS
#   error It is necessary for right property pages handling.
#endif
int CMiscSettings::OnApply()
{
  // PSNRET_NOERROR = apply OK
  // PSNRET_INVALID = apply not OK, return to this page
  // PSNRET_INVALID_NOCHANGEPAGE = apply not OK, don't change focus
  int ret = DoDataExchange(TRUE) ? PSNRET_NOERROR : PSNRET_INVALID;

  if ( PSNRET_NOERROR == ret )
  {
    m_storage.Save();
    CValues::Assign( m_storage );
  }
  return ret;
}
