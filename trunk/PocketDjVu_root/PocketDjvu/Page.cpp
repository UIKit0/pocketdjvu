#include "StdAfx.h"

#include "Page.h"
#include "PageLoader.h"

bool CPage::LoadBmpSync()
{  
  CPageLoader bmpLdr( m_hWnd,
                      m_pDjVuDoc,
                      m_pageIndex,
                      m_rc.Width(),
                      m_rc.Height(),
                      m_bWidthMostPrioritized );
  if ( !bmpLdr.LoadBmp() )
    return false;

  bmpLdr.StoleBmp( m_Bmp, m_rc );
  m_ImgWidth  = bmpLdr.GetImgWidth();
  m_ImgHeight = bmpLdr.GetImgHeight();

  return true;
}

void CPage::Draw( CDCHandle dc )
{
  CDC bmpDC;
  bmpDC.CreateCompatibleDC( dc );

  if ( m_Bmp )
  {
    HBITMAP origBmp = bmpDC.SelectBitmap( m_Bmp );
    dc.BitBlt( m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), bmpDC, 0, 0, SRCCOPY );
    bmpDC.SelectBitmap( origBmp );
  }
}