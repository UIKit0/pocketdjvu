#pragma once

WTL::CString GetModuleVersionStr( HMODULE hModule );

void ShowNotification( HWND hwndSink, wchar_t const * szCaption, wchar_t const * szBodytext );
void NotificationRemove();

template <typename T, typename _XData>
inline void NotifyDataValidateError( HWND hwnd, UINT nCtrlID, BOOL bSave, _XData & data )
{
  if ( !bSave )
  {
    return;
  }

  WTL::CString msg;
  switch ( data.nDataType )
  {
  case T::ddxDataText:
    msg.LoadString( IDS_TOO_LONG_TEXT_VALUE );
  break;
  
  case T::ddxDataInt:
    msg.Format( IDS_INT_VAL_WRONG, data.intData.nVal );
  break;
  
  case T::ddxDataFloat:
  case T::ddxDataDouble:
    msg.Format( IDS_DOUBLE_VAL_WRONG, data.floatData.nVal );
  break;

  default:
    return;
  }
  
  WTL::CString wrn;
  wrn.LoadString( IDS_WARNING );
  ShowNotification( hwnd, wrn, msg );
}

BOOL CreateDlgMenuBar( UINT nToolBarId, HWND hWndParent );

HWND FindChildWndByClassName( HWND hWndParent, 
                              wchar_t const * pClassName,
                              bool bShallow=true );

bool IsVGA();