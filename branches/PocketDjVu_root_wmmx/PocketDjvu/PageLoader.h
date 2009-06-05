#pragma once

#include "./Values.h"

#include "../libdjvu/DjVuDocument.h"

namespace DJVU
{
  class GPixmap;
  class GBitmap;
}

//-----------------------------------------------------------------------------
class CPageLoader
{
public:
  CPageLoader( HWND hWnd, 
               GP<DjVuDocument> pDjVuDoc, 
               int pageIndex,
               int width,
               int height,
               bool bWidthMostPrioritized,
               int rotation = 0 // CCW: 90degree * rotation
             );
  ~CPageLoader();

  enum RET_CODES { RET_OK, RET_ERROR, RET_EMPTY };
  RET_CODES LoadBmp();  

  void StoleBmp( BITMAPINFO *& destpBmp, WTL::CRect & rc ) // throw()
  {
    rc.right  = rc.left + m_Width;
    rc.bottom = rc.top  + m_Height;

    destpBmp = m_pBmp;
    m_pBmp = 0;
  }

  int GetWidth() const
  {
    return m_Width;
  }

  int GetHeight() const
  {
    return m_Height;
  }

  int GetImgWidth() const
  {
    return m_ImgWidth;
  }

  int GetImgHeight() const
  {
    return m_ImgHeight;
  }

private:
  RET_CODES LoadBmpImp();

  void CalcDimensions( int imgWidth,
                       int imgHeight,
                       int & o_Width,
                       int & o_Height );

  bool ConstructDIBbyPixmap( int width, int height, GPixmap & pixm );
  bool ConstructDIBbyBitmap( int width, int height, GBitmap & bitmap );

private:
  HWND m_hWnd;
  DJVU::GP<DJVU::DjVuDocument> m_pDjVuDoc;
  int m_pageIndex;
  int m_Width;
  int m_Height;
  int m_ImgWidth;
  int m_ImgHeight;
  bool m_bWidthMostPrioritized;
  BITMAPINFO * m_pBmp;
  /// CCW: 90degree * m_rotation
  int m_rotation;
};
