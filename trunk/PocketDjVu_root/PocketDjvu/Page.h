#pragma once

#include "./Values.h"
#include "../libdjvu/DjVuDocument.h"
#include "IPage.h"
//-----------------------------------------------------------------------------
class CPage : public IPage
{
public:
  CPage( HWND hWnd,
         GP<DjVuDocument> pDjVuDoc,
         WTL::CRect const & rect,
         bool bWidthMostPrioritized,
         int pageIndex ) : 
    m_hWnd(hWnd)
    , m_pDjVuDoc(pDjVuDoc)
    , m_pBmp()
    , m_bWidthMostPrioritized(bWidthMostPrioritized)
    , m_rc(rect)
    , m_ImgWidth()
    , m_ImgHeight()
    , m_pageIndex(pageIndex)
  {
    m_rc.NormalizeRect();
  }
    
  virtual ~CPage()
  {
    if ( m_pBmp )
    {
      siv::vm_free( m_pBmp, 1 );
    }
  }

  virtual int GetPageIndex() const
  {
    return m_pageIndex;
  }

  virtual WTL::CRect const & GetRect() const
  {
    return m_rc;
  }

  virtual void GetImgSize( int & o_width, int & o_height  ) const
  {
    o_width  = m_ImgWidth;
    o_height = m_ImgHeight;
  }

  virtual void Scroll( int dx, int dy )
  {
    m_rc.left   += dx;
    m_rc.right  += dx;
    m_rc.top    += dy;
    m_rc.bottom += dy;
  }

  virtual bool LoadBmpSync();  

  virtual bool LoadBmpAsync()
  {
    // TODO: create really asynchronous code.
    return LoadBmpSync();
  }

  virtual void Draw( WTL::CDCHandle dc );
  
private:
  CPage & operator =( CPage const & );
  CPage( CPage const & );

private:
  HWND m_hWnd;
  GP<DjVuDocument> m_pDjVuDoc;
  BITMAPINFO * m_pBmp;
  WTL::CRect   m_rc;
  int m_ImgWidth;
  int m_ImgHeight;
  int m_pageIndex;
  bool m_bWidthMostPrioritized;
};
