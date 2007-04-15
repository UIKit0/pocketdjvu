#pragma once

#include "./misc.h"
//-----------------------------------------------------------------------------
/// NB: Keep it as POD type.
class CBookmarkInfo
{
public:
  CBookmarkInfo()
  {
    Reset();
  }

  CBookmarkInfo( int pageIndex
                 , WTL::CRect const & pageRect
                 , bool bPortrait = IsPortrait() ) :
    m_pageIndex(pageIndex),
    m_pageRect(pageRect),
    m_bPortrait(bPortrait)
  {
  }

  void Reset()
  {
    m_pageIndex = 0;
    
    if ( SystemParametersInfo( SPI_GETWORKAREA, 0, &m_pageRect, 0 ) )
    {
      m_pageRect.OffsetRect( -m_pageRect.left, -m_pageRect.top );
    }
    else
    {
      m_pageRect = WTL::CRect( 0, 0, 320, 200 );
    }

    m_bPortrait = IsPortrait();
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
    
    ULONG nBytes = sizeof(m_pageRect);
    res = ERROR_SUCCESS == key.QueryBinaryValue( L"Rect", &m_pageRect, &nBytes ) && res;
    if ( res )
    {
      ATLASSERT( sizeof(m_pageRect) == nBytes );
    }

    val = m_bPortrait;
    res = ERROR_SUCCESS == key.QueryDWORDValue( L"bPortrait", val ) && res;
    if ( m_bPortrait != !!val )
    {
      CheckPortrait();
    }
    return res;
  }

private:
  void CheckPortrait()
  {
    double k = double(GetSystemMetrics(SM_CXSCREEN)) / GetSystemMetrics(SM_CYSCREEN);
    m_pageRect.left   = Round( k * m_pageRect.left );
    m_pageRect.top    = Round( k * m_pageRect.top );
    m_pageRect.right  = Round( k * m_pageRect.right );
    m_pageRect.bottom = Round( k * m_pageRect.bottom );
  }

public:
// DATA:
  int m_pageIndex;
  WTL::CRect m_pageRect;
  bool m_bPortrait;
};
