#include "StdAfx.h"

#include <vector>

#include "resource.h"
#include "./misc.h"

WTL::CString GetModuleVersionStr( HMODULE hModule )
{
  wchar_t fname[ MAX_PATH ];
  DWORD len = ::GetModuleFileName( hModule, fname, sizeof(fname)/sizeof(fname[0]) );
  if ( !len )
  {
    return L"";
  }

  DWORD t = 0;
  len = ::GetFileVersionInfoSize( fname, &t );
  if ( !len )
  {
    return L"";
  }

  std::vector<char> buf;
  buf.resize( len );
  BOOL res = ::GetFileVersionInfo( fname, 0, len, &buf[0] );
  if ( !res )
    return L"";
  
  VS_FIXEDFILEINFO * pInf = 0;
  UINT lInf = 0;
  WTL::CString sVer;
  
  res = ::VerQueryValue( &buf[0], L"\\", (LPVOID*)&pInf, &lInf );
  if ( !res )
    return L"";
  
  WTL::CString s;
  s.Format( L"%d", int(pInf->dwProductVersionMS >> 16) );
  sVer += s + L".";

  s.Format( L"%d", int(pInf->dwProductVersionMS & 0xFFFF) );
  sVer += s + L".";

  s.Format( L"%d", int(pInf->dwProductVersionLS >> 16) );
  sVer += s + L".";

  s.Format( L"%d", int(pInf->dwProductVersionLS & 0xFFFF) );
  sVer +=  s;

  return sVer;
}

//-----------------------------------------------------------------------------
// {7BCA3190-2E25-4dd3-A309-C948C8379682}
static const GUID notifyGUID = 
{ 0x7bca3190, 0x2e25, 0x4dd3, { 0xa3, 0x9, 0xc9, 0x48, 0xc8, 0x37, 0x96, 0x82 } };

void NotificationRemove()
{
  SHNotificationRemove( &notifyGUID, ID_NOFIFY_1 );
}

void ShowNotification( HWND hwndSink, wchar_t const * szCaption, wchar_t const * szBodytext )
{
  SHNOTIFICATIONDATA sn  = {0};

  sn.cbStruct = sizeof(sn);
  sn.dwID = ID_NOFIFY_1;
  sn.npPriority = SHNP_INFORM;
  sn.csDuration = 11;
  sn.hicon = ::LoadIcon( _Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME) );
  sn.grfFlags = SHNF_FORCEMESSAGE;
  sn.clsid = notifyGUID;
  sn.hwndSink = hwndSink;
  
  WTL::CString txt = L"<html><body>";
  txt += szBodytext;
  
  WTL::CString hideStrFormat;
  hideStrFormat.LoadString( IDS_DISMISS_LINK );
  
  WTL::CString hideCmd;
  hideCmd.Format( hideStrFormat, int(ID_NOFIFY_1) );

  txt += hideCmd;
  txt += L"</html></body>";
  sn.pszHTML = txt;

  WTL::CString cap = szCaption;
  sn.pszTitle = cap;

#if WINVER > 0x0420
  WTL::CString dismiss;
  dismiss.LoadString( IDS_DISMISS );
  sn.rgskn[0].pszTitle = dismiss;
  sn.rgskn[0].skc.wpCmd = ID_NOFIFY_1;
#endif

  SHNotificationAdd( &sn );
}

//-----------------------------------------------------------------------------
BOOL CreateDlgMenuBar( UINT nToolBarId, HWND hWndParent )
{
  ATLASSERT( hWndParent );

  SHMENUBARINFO mbi = { 0 };
  mbi.cbSize      = sizeof(mbi);
  mbi.hwndParent  = hWndParent;
  mbi.dwFlags     = SHCMBF_HMENU;
  mbi.nToolBarId  = nToolBarId;
  mbi.hInstRes    = ATL::_AtlBaseModule.GetModuleInstance();

  BOOL bRet = ::SHCreateMenuBar( &mbi );
  //if( bRet )
  //{
  //  HWND m_hWndMnuBar = mbi.hwndMB;
  //}

  return bRet;
}

//-----------------------------------------------------------------------------
static HWND FindChildWndByClassNameImp( size_t clL,
                                        HWND hWndParent, 
                                        wchar_t const * pClassName,
                                        bool bShallow )
{
  wchar_t * szBuf = 0;
  HWND hLookingFor = 0;
  HWND hNext = GetWindow( hWndParent, GW_CHILD );
  while( hNext )
  {
    if ( !szBuf )
    {
      __try
      {
        szBuf = (wchar_t*)alloca( clL * sizeof(wchar_t) );
      }
      __except( GetExceptionCode() == STATUS_STACK_OVERFLOW )
      {
        return 0;
      }
    }
    if ( !GetClassNameW( hNext, szBuf, clL ) )
    {
      DWORD err = GetLastError(); // ? ERROR_INSUFFICIENT_BUFFER    122L
      return 0;
    }

    if ( !_wcsicmp( szBuf, pClassName ) )
    {
      return hNext;
    }

    if ( !bShallow )
    {
      hLookingFor = FindChildWndByClassNameImp( clL, hNext, pClassName, bShallow );
      if ( hLookingFor )
      {
        return hLookingFor;
      }
    }    

    hNext = ::GetWindow( hNext, GW_HWNDNEXT );
  }

  if ( !bShallow )
  {
    return 0;
  }

  hNext = GetWindow( hWndParent, GW_CHILD );
  while( hNext )
  {
    hLookingFor = FindChildWndByClassNameImp( clL, hNext, pClassName, bShallow );
    if ( hLookingFor )
    {
      return hLookingFor;
    }
    hNext = ::GetWindow( hNext, GW_HWNDNEXT );
  }

  return 0;
}

HWND FindChildWndByClassName( HWND hWndParent, wchar_t const * pClassName, bool bShallow )
{
  // +1 - if some window class has the name with the same leading characters as passed in the pClassName
  //      we have to add at least 1 extra character to have possibility distinguish this case.
  size_t clL = wcslen(pClassName) + 1 + 1; // +1 - Zero terminal
  return FindChildWndByClassNameImp( clL, hWndParent, pClassName, bShallow );
}

bool IsVGA()
{
  bool bVGA = max( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ) > 320;
  return bVGA;
}