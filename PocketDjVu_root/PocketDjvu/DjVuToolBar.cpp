#include "StdAfx.h"

#include "./resource.h"
#include "./DjVuToolBar.h"
#include "./misc.h"
#include "./constants.h"

#include <strsafe.h>

bool CDjVuToolBar::SubclassToolbar( HWND hWndMenuBar )
{
  m_hWndSIP = FindChildWndByClassName( NULL, L"MS_SIPBUTTON" );
  if ( !m_hWndSIP )
  {    
    return false;
  }  

  HWND hWndToolBar = FindChildWndByClassName( hWndMenuBar, L"ToolbarWindow32" );
  if ( !hWndToolBar )
  {
    return false;
  }

  if ( !SubclassWindow( hWndToolBar ) )
  {
    return false;
  }

  m_back.LoadBitmap( MAKEINTRESOURCE(IsVGA()?IDB_BACK_VGA:IDB_BACK_QVGA) );
  m_forward.LoadBitmap( MAKEINTRESOURCE(IsVGA()?IDB_FORWARD_VGA:IDB_FORWARD_QVGA) );

  return true;
}

bool CDjVuToolBar::GetOutRect( RECT * rect )
{
  WTL::CRect sipR;
  if ( !::GetWindowRect( m_hWndSIP, sipR ) )
  {
    return false;
  }
  ScreenToClient( &sipR.TopLeft() );
  ScreenToClient( &sipR.BottomRight() );

  RECT & r = *rect;  
  int btnCnt = GetButtonCount();
  ATLASSERT( btnCnt );
  --btnCnt;
  if ( !GetItemRect( btnCnt, &r ) )
  {
    return 0;
  }
  
  if ( sipR.left <= r.right )
  {
    return false;
  }
  r.left  = r.right + g_cToolBarOuterGap;
  r.right = sipR.left - g_cToolBarOuterGap;
  r.bottom  = sipR.bottom;

  return true;
}

bool CDjVuToolBar::DrawArrow( WTL::CDC & dc, WTL::CBitmap const & bmp, WTL::CRect r )
{
  if ( !bmp )
  {
    return false;
  }

  WTL::CDC bdc;
  if ( !bdc.CreateCompatibleDC( dc ) )
  {
    return false;
  }

  HBITMAP oldBmp = bdc.SelectBitmap( bmp );
  WTL::CSize sz;
  bmp.GetSize( sz );
  BLENDFUNCTION bf = {0};
  bf.BlendOp = AC_SRC_OVER;
  bf.SourceConstantAlpha = 255;
  bf.AlphaFormat = AC_SRC_ALPHA;

  BOOL res = AlphaBlend( dc,  r.left, r.top, r.Width(), r.Height(),
                         bdc, 0,      0,     sz.cx,     sz.cy,
                         bf );
  bdc.SelectBitmap( oldBmp );
  return !!res;
}

void CDjVuToolBar::DrawHistoryButtond( WTL::CDC & dc, WTL::CRect & panelRect )
{
  WTL::CRect r   = panelRect;
  r.right        = r.left  + panelRect.Height()/2;
  panelRect.left = r.right + g_cGapBetweenRectAndInfoZone;

  WTL::CPen pen( (HPEN)GetStockObject(BLACK_PEN) );
  WTL::CBrush br( (HBRUSH)GetStockObject(BLACK_BRUSH) );

  HPEN hOldPen = dc.SelectPen( pen );
  HBRUSH hOldBr = dc.SelectBrush( br );
  
  if ( !DrawArrow( dc,
                   m_back, 
                   WTL::CRect( r.left,
                               r.top, 
                               r.left+r.Height()/2,
                               r.top+r.Height()/2
                             )
                 )
     )
  {
    POINT points[] = { {r.left,  r.top+r.Height()/4},
                       {r.right, r.top},
                       {r.right, r.top+r.Height()/2}
                     };
    dc.Polygon( points, sizeof(points)/sizeof(points[0]));
  }

  if ( !DrawArrow( dc,
                   m_forward, 
                   WTL::CRect( r.left,
                               r.top+r.Height()/2, 
                               r.left+r.Height()/2,
                               r.top+r.Height()
                             )
                 )
     )
  {
    POINT points[] = { {r.left,  r.top+r.Height()/2},
                       {r.right, r.top+r.Height()*3/4},
                       {r.left,  r.top+r.Height()}
                     };
    dc.Polygon( points, sizeof(points)/sizeof(points[0]));
  }

  dc.SelectBrush( hOldBr );
  dc.SelectPen( hOldPen );
}

LRESULT CDjVuToolBar::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  DefWindowProc();

  if ( !m_curPg || !m_numPg )
  {
    return 0;
  }

  WTL::CDC dc = GetDC();

  WTL::CRect r;
  if ( !GetOutRect( &r ) )
  {
    return 0;
  }
  
  {
    WTL::CFont f;
    LOGFONT lf = {0};
    lf.lfHeight = -g_cPgFontHpx;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    
    dc.DrawEdge( &r, BDR_SUNKENOUTER, BF_MONO|BF_RIGHT );
    DrawHistoryButtond( dc, r );

    r.DeflateRect( g_cToolBarInnerGap, g_cToolBarInnerGap );
    wchar_t str[ 16 ];
    StringCbPrintf( str, sizeof(str), L"%d/%d pg.", m_curPg, m_numPg );
    if ( f.CreateFontIndirect( &lf ) )
    {
      HFONT olfF = dc.SelectFont( f );
      int oldBkMode = dc.SetBkMode( TRANSPARENT );
      COLORREF oldCol = dc.SetTextColor( ::GetSysColor(COLOR_BTNTEXT) );
      
      dc.DrawTextW( str, -1, &r, DT_LEFT|DT_TOP|DT_NOPREFIX|DT_WORDBREAK );
      
      dc.SetTextColor( oldCol );
      dc.SetBkMode( oldBkMode );
      dc.SelectFont( olfF );
    }
  }
  return 0;
}

LRESULT CDjVuToolBar::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  WTL::CRect r;
  if ( !GetOutRect( &r ) )
  {
    bHandled  = false;
    return 0;
  }

  WTL::CPoint p( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );

  BTN_ZONE zone = TestBtnZone( p, r );
  switch ( zone )
  {
    case PANE:
    {
      SHRGINFO shrg = {0};
      shrg.cbSize     = sizeof shrg;
      shrg.hwndClient = m_hWnd;
      shrg.ptDown     = p;
      shrg.dwFlags    = SHRG_RETURNCMD;
      if ( GN_CONTEXTMENU != SHRecognizeGesture( &shrg ) )
      {
        ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_GOTOPAGE, 0 ); 
      }
      else
      {
        m_bPostHistoryCommand = true;
      }
    }
    break;

    case BACK:
      ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_BACK, 0 ); 
    break;

    case FORWARD:
      ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_FORWARD, 0 ); 
    break;

    default:
      bHandled  = false;
    break;
  }
  return 0;
}

LRESULT CDjVuToolBar::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{  
  if ( m_bPostHistoryCommand )
  {
    m_bPostHistoryCommand = false;
    ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_HISTORY, 0 ); 
    return 0;
  }
  bHandled  = false;
  return 0;
}

void CDjVuToolBar::SetPages( int curPg, int numPg )
{
  if ( m_curPg == curPg && m_numPg == numPg )
  {
    return;
  }

  m_curPg   = curPg;
  m_numPg = numPg;

  WTL::CRect r;
  if ( !GetOutRect( &r ) )
  {
    return;
  }
  InvalidateRect( &r );
}

CDjVuToolBar::BTN_ZONE CDjVuToolBar::TestBtnZone( WTL::CPoint p, WTL::CRect const & panelRect ) const
{
  if ( !panelRect.PtInRect( p ) )
  {
    return NOPE;
  }

  WTL::CRect r   = panelRect;
  r.right        = r.left  + panelRect.Height()/2;

  if ( r.right < p.x )
  {
    return PANE;
  }

  if ( r.Height()/2 < p.y )
  {
    return FORWARD;
  }

  return BACK;
}

POINT CDjVuToolBar::GetPointForMenu()
{
  WTL::CRect r;
  GetOutRect( &r );
  
  //r.right = g_cGapBetweenRectAndInfoZone + r.left + r.Height()/2;

  WTL::CPoint p( r.TopLeft() );
  ClientToScreen( &p );
  return p;
}
