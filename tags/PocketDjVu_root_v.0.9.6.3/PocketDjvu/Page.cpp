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
  CPageLoader::RET_CODES res = bmpLdr.LoadBmp();
  if ( CPageLoader::RET_OK != res && CPageLoader::RET_EMPTY != res )
    return false;

  bmpLdr.StoleBmp( m_pBmp, m_rc );
  m_ImgWidth  = bmpLdr.GetImgWidth();
  m_ImgHeight = bmpLdr.GetImgHeight();

  return true;
}

void CPage::Draw( WTL::CDCHandle dc, int dx, int dy )
{
  if ( !m_pBmp )
  {
    dc.FillRect( m_rc, (HBRUSH)GetStockObject(WHITE_BRUSH) );
    return;
  }
  void * pvBits =(char*)m_pBmp + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*m_pBmp->bmiHeader.biClrUsed;
  int lines = dc.SetDIBitsToDevice(
                dx + m_rc.left, dy + m_rc.top,
                m_rc.Width(), m_rc.Height(), 
                0, 0, 
                0, m_pBmp->bmiHeader.biHeight,
                pvBits,
                m_pBmp,
                DIB_RGB_COLORS
              );
}