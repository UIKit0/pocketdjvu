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
  WTL::CString hideStr;
  hideStr.LoadString( IDS_DISMISS_LINK );
  WTL::CString hideCmd;
  hideCmd.Format( L"<br><a href=\"cmd:%d\">Dismiss</a>.", int(ID_NOFIFY_1) );
  txt += hideCmd;
  txt += L"</html></body>";
  sn.pszHTML = txt;
  sn.pszTitle = szCaption;

#if WINVER > 0x0420
  WTL::CString dismiss;
  dismiss.LoadString( IDS_DISMISS );
  sn.rgskn[0].pszTitle = dismiss;
  sn.rgskn[0].skc.wpCmd = ID_NOFIFY_1;
#endif

  SHNotificationAdd( &sn );
}