#pragma once

//-----------------------------------------------------------------------------
/// NB: Keep it as POD type.
struct CBookmarkInfo
{
  CBookmarkInfo()
  {
    Reset();
  }

  CBookmarkInfo( int pageIndex
                 , WTL::CRect const & pageRect
                 , bool bPortrait = true ) :
    m_pageIndex(pageIndex)
    , m_pageRect(pageRect)
    , m_bPortrait(bPortrait)
  {
  }

  void Reset()
  {
    m_pageIndex = 0;
    m_bPortrait = true;
    
    if ( SystemParametersInfo( SPI_GETWORKAREA, 0, &m_pageRect, 0 ) )
    {
      m_pageRect.OffsetRect( -m_pageRect.left, -m_pageRect.top );
    }
    else
    {
      m_pageRect = WTL::CRect( 0, 0, 320, 200 );
    }    
  }

  bool SaveToReg( HKEY hKeyParent, wchar_t const * szName ) const throw()
  {
    ATL::CRegKey key;
    key.Create( hKeyParent, szName );
    if ( !key )
    {
      return false;
    }

    bool res = true;
    res = ERROR_SUCCESS == key.SetDWORDValue( L"Page", m_pageIndex+1 ) && res;
    res = ERROR_SUCCESS == key.SetDWORDValue( L"bPortrait", m_bPortrait ) && res;
    res = ERROR_SUCCESS == key.SetBinaryValue( L"Rect", &m_pageRect, sizeof m_pageRect ) && res;
    return res;
  }

  bool LoadFromReg( HKEY hKeyParent, wchar_t const * szName ) throw()
  {
    Reset();

    ATL::CRegKey key;
    if ( ERROR_SUCCESS != key.Open( hKeyParent, szName ) )
    {
      return false;
    }

    bool res = true;
    DWORD val = 0;
    res = ERROR_SUCCESS == key.QueryDWORDValue( L"Page", val ) && res;
    m_pageIndex = val - 1;

    val = 1;
    res = ERROR_SUCCESS == key.QueryDWORDValue( L"bPortrait", val ) && res;
    m_bPortrait = !!val;

    ULONG nBytes = sizeof(m_pageRect);
    res = ERROR_SUCCESS == key.QueryBinaryValue( L"Rect", &m_pageRect, &nBytes ) && res;
    if ( res )
    {
      ATLASSERT( sizeof(m_pageRect) == nBytes );
    }

    return res;
  }

// DATA:
  int m_pageIndex;
  WTL::CRect m_pageRect;
  bool m_bPortrait;
};
