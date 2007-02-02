#include "StdAfx.h"

#include <algorithm>
#include <stdexcept>

#include "../libdjvu/GBitmap.h"
#include "../libdjvu/DjVuImage.h"

#include "PageLoader.h"

CPageLoader::CPageLoader( HWND hWnd, 
                          GP<DjVuDocument> pDjVuDoc, 
                          int pageIndex,
                          int width,
                          int height,
                          bool bWidthMostPrioritized,
                          int rotation
                        )
  : m_hWnd(hWnd)
  , m_pDjVuDoc(pDjVuDoc)
  , m_pageIndex(pageIndex)
  , m_bWidthMostPrioritized(bWidthMostPrioritized)
  , m_Width(width)
  , m_Height(height)
  , m_ImgWidth()
  , m_ImgHeight()
  , m_pBmp()
  , m_rotation(rotation)
{
}

CPageLoader::~CPageLoader()
{
  if ( m_pBmp )
  {
    siv::vm_free( m_pBmp );
  }
}

bool CPageLoader::LoadBmp()
{
  __try
  {
    return LoadBmpImp();
  }
  __except( EXCEPTION_EXECUTE_HANDLER )
  {
    return false;
  }
}
bool CPageLoader::LoadBmpImp()
{
  try
  {
    using namespace DJVU;
    GP<DjVuImage> pImage = m_pDjVuDoc->get_page( m_pageIndex );
    
    pImage->set_rotate( m_rotation );

    if ( !pImage->wait_for_complete_decode() )
      return false;

    GP<DjVuInfo> pInfo = pImage->get_info();
    m_ImgWidth  = pInfo->width;
    m_ImgHeight = pInfo->height;
    if ( !m_ImgWidth || !m_ImgHeight )
      return false;

    if ( m_rotation & 1 )
    {
      std::swap( m_ImgWidth, m_ImgHeight );
    }

    CalcDimensions( m_ImgWidth, m_ImgHeight, m_Width, m_Height );

    GP<GPixmap> pPixm = pImage->get_pixmap( GRect(0,0,m_Width,m_Height),
                                            GRect(0,0,m_Width,m_Height) );
	  if ( pPixm )
    {
      if ( !ConstructDIBbyPixmap( m_Width, m_Height, *pPixm ) )
        return false;
    }
    else
    {
		  GP<GBitmap> pBmp = pImage->get_bitmap( GRect(0,0,m_Width,m_Height),
                                             GRect(0,0,m_Width,m_Height) );
      if ( !pBmp || !ConstructDIBbyBitmap( m_Width, m_Height, *pBmp ) )
      {
        return false;
      }
    }    
    return true;
  }
  catch ( std::exception const & e )
  {
    e;
    // TODO: output diagnostics
    return false;
  }
  catch( GException & e )
  {
    e;
    // TODO: output diagnostics
    return false;
  }
  catch(...)
  {
    // TODO: output diagnostics
    return false;
  }
}

void CPageLoader::CalcDimensions( int imgWidth,
                                  int imgHeight,
                                  int & o_Width,
                                  int & o_Height )
{
  double r = 1.;
  if ( m_bWidthMostPrioritized )
  {
    r = ((double)m_Width)/imgWidth;
  }
  else
  {
    r = ((double)m_Height)/imgHeight;
  }
  o_Width  = int(imgWidth*r+0.5);
  o_Height = int(imgHeight*r+0.5);
}

bool CPageLoader::ConstructDIBbyPixmap( int width, int height, GPixmap & pixm )
{
  ATLASSERT( pixm.columns() == width );
  ATLASSERT( pixm.rows() == height );

  unsigned const HEADER_SIZE = sizeof(BITMAPINFOHEADER);
  int bytesInRow = ((width*3+3)>>2)<<2;
  unsigned bitmapSize = HEADER_SIZE + bytesInRow*height;

  m_pBmp = (BITMAPINFO *)siv::vm_malloc( bitmapSize );
  ZeroMemory( m_pBmp, HEADER_SIZE );

  m_pBmp->bmiHeader.biSize              = sizeof BITMAPINFOHEADER;
  m_pBmp->bmiHeader.biWidth             = width;
  m_pBmp->bmiHeader.biHeight            = height;
  m_pBmp->bmiHeader.biPlanes            = 1;
  m_pBmp->bmiHeader.biBitCount          = 24;
  m_pBmp->bmiHeader.biCompression       = BI_RGB;
  m_pBmp->bmiHeader.biSizeImage         = 0;
  m_pBmp->bmiHeader.biXPelsPerMeter     = 3600;
  m_pBmp->bmiHeader.biYPelsPerMeter     = 3600;
  m_pBmp->bmiHeader.biClrUsed           = 0;
  m_pBmp->bmiHeader.biClrImportant      = 0;

  int const ct_asseert = 1/int(sizeof(GPixel)==3);
  
  unsigned char * pPix = (unsigned char*)m_pBmp + HEADER_SIZE;
  
  for ( int i=0; i<height; ++i )
  {
    GPixel * pRow = pixm[i];
    memcpy( pPix, pRow, width*3 );
    pPix += bytesInRow;
  }

  return true;
}

bool CPageLoader::ConstructDIBbyBitmap( int width, int height, GBitmap & bitmap )
{
  ATLASSERT( bitmap.columns() == width );
  ATLASSERT( bitmap.rows() == height );
  
  unsigned const HEADER_SIZE = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
  int bytesInRow = ((width+3)>>2)<<2;
  unsigned bitmapSize = HEADER_SIZE + bytesInRow*height;

  m_pBmp = (BITMAPINFO *)siv::vm_malloc( bitmapSize );
  ZeroMemory( m_pBmp, HEADER_SIZE );

  m_pBmp->bmiHeader.biSize              = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
  m_pBmp->bmiHeader.biWidth             = width;
  m_pBmp->bmiHeader.biHeight            = height;
  m_pBmp->bmiHeader.biPlanes            = 1;
  m_pBmp->bmiHeader.biBitCount          = 8;
  m_pBmp->bmiHeader.biCompression       = BI_RGB;
  m_pBmp->bmiHeader.biSizeImage         = 0;
  m_pBmp->bmiHeader.biXPelsPerMeter     = 3600;
  m_pBmp->bmiHeader.biYPelsPerMeter     = 3600;
  m_pBmp->bmiHeader.biClrUsed           = 256;
  m_pBmp->bmiHeader.biClrImportant      = 0;

  for ( int i=0; i<256; ++i )
  {
    m_pBmp->bmiColors[i].rgbBlue      = 255 - i;
    m_pBmp->bmiColors[i].rgbGreen     = 255 - i;
    m_pBmp->bmiColors[i].rgbRed       = 255 - i;
    m_pBmp->bmiColors[i].rgbReserved  = 0;
  }

  unsigned char * pPix = (unsigned char*)m_pBmp + HEADER_SIZE;
  for ( int i=0; i<height; ++i )
  {
    unsigned char * pRow = bitmap[i];
    memcpy( pPix, pRow, width );
    pPix += bytesInRow;
  }
  return true;
}
