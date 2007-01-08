#include "StdAfx.h"

#include <vector>

#include "./misc.h"

CString GetModuleVersionStr( HMODULE hModule )
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
  CString sVer;
  
  res = ::VerQueryValue( &buf[0], L"\\", (LPVOID*)&pInf, &lInf );
  if ( !res )
    return L"";
  
  CString s;
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