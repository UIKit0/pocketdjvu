#pragma once

//-----------------------------------------------------------------------------
/// NB: Keep it as POD type.
struct CBookmarkInfo
{
  CBookmarkInfo( int pageIndex
                 , WTL::CRect const & pageRect
                 , bool bPortrait = true ) :
    m_pageIndex(pageIndex)
    , m_pageRect(pageRect)
    , m_bPortrait(bPortrait)
  {
  }

  bool SaveToReg( HKEY hKey ) throw()
  {
    ATL::CRegKey key;
    key.Attach( hKey );

    bool res = true;
    res = res && ERROR_SUCCESS == key.SetDWORDValue( L"Page", m_pageIndex+1 );
    res = res && ERROR_SUCCESS == key.SetDWORDValue( L"bPortrait", m_bPortrait );
    res = res && ERROR_SUCCESS == key.SetBinaryValue( L"Rect", &m_pageRect, sizeof m_pageRect );

    key.Detach();
    return res;
  }

  bool LoadFromReg( HKEY hKey ) throw()
  {
    ATL::CRegKey key;
    key.Attach( hKey );

    bool res = true;
    DWORD val = 0;
    res = res && ERROR_SUCCESS == key.QueryDWORDValue( L"Page", val );
    m_pageIndex = val - 1;
    res = res && ERROR_SUCCESS == key.QueryDWORDValue( L"bPortrait", val );
    m_bPortrait = !!val;
    ULONG nBytes;
    res = res && ERROR_SUCCESS == key.QueryBinaryValue( L"Rect", &m_pageRect, &nBytes );
    if ( res )
    {
      ATLASSERT( sizeof(m_pageRect) == nBytes );
    }

    key.Detach();
    return res;
  }

// DATA:
  int m_pageIndex;
  WTL::CRect m_pageRect;
  bool m_bPortrait;
};
