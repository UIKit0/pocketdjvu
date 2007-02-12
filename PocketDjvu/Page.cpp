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

  bmpLdr.StoleBmp( m_pBmp, m_rc );
  m_ImgWidth  = bmpLdr.GetImgWidth();
  m_ImgHeight = bmpLdr.GetImgHeight();

  return true;
}

void CPage::Draw( WTL::CDCHandle dc )
{
  if ( !m_pBmp )
  {
    return;
  }
  void * pvBits =(char*)m_pBmp + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*m_pBmp->bmiHeader.biClrUsed;
  int lines = dc.SetDIBitsToDevice(
                m_rc.left, m_rc.top,
                m_rc.Width(), m_rc.Height(), 
                0, 0, 
                0, m_pBmp->bmiHeader.biHeight,
                pvBits,
                m_pBmp,
                DIB_RGB_COLORS
              );
}