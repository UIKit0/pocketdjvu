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
  r.left  = r.right + g_cTollBarGap;
  r.right = sipR.left - g_cTollBarGap;
  r.bottom  = sipR.bottom;

  return true;
}

static void DrawHistoryButtond( WTL::CDC & dc, WTL::CRect & panelRect )
{
  WTL::CRect r    = panelRect;
  ++r.left;
  r.right         = r.left + (IsVGA() ? 16 : 8);
  panelRect.left  = r.right;

  WTL::CPen pen( (HPEN)GetStockObject(BLACK_PEN) );
  WTL::CBrush br( (HBRUSH)GetStockObject(BLACK_BRUSH) );

  HPEN hOldPen = dc.SelectPen( pen );
  HBRUSH hOldBr = dc.SelectBrush( br );
  
  {
    POINT points[] = { {r.left,  r.top+r.Height()/4},
                       {r.right, r.top},
                       {r.right, r.top+r.Height()/2}
                     };
    // TODO: draw special icon?
    dc.Polygon( points, sizeof(points)/sizeof(points[0]));
  }
  {
    POINT points[] = { {r.left,  r.top+r.Height()/2},
                       {r.right, r.top+r.Height()*3/4},
                       {r.left,  r.top+r.Height()}
                     };
    // TODO: draw special icon?
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
    
    dc.DrawEdge( &r, BDR_SUNKENINNER, BF_RECT );
    DrawHistoryButtond( dc, r );

    r.DeflateRect( 3, 3 );
    wchar_t str[ 32 ];
    StringCchPrintf( str, sizeof(str)/sizeof(str[0]), L"%d/%d pg.", m_curPg, m_numPg );
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
  bHandled  = false;
  
  WTL::CRect r;
  if ( !GetOutRect( &r ) )
  {
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
        //TODO: ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_SHOW_HISTORY, lParam ); 
      }
    }
    break;

    case BACK:
      //TODO: ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_BACK, 0 ); 
    break;

    case FORWARD:
      //TODO: ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_FORWARD, 0 ); 
    break;
  }
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

  WTL::CRect r    = panelRect;
  r.right         = 1+r.left + (IsVGA() ? 16 : 8);

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
