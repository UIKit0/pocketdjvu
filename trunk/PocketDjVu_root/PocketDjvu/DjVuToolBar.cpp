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
  //WTL::CPen pen( (HPEN)GetStockObject(BLACK_PEN) );

  //HPEN hOldPen = dc.SelectPen( pen );
  {
    WTL::CFont f;
    LOGFONT lf = {0};
    lf.lfHeight = -g_cPgFontHpx;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    
    dc.DrawEdge( &r, BDR_SUNKENINNER, BF_RECT );

    r.DeflateRect( 3, 3 );
    wchar_t str[ 32 ];
    StringCchPrintf( str, sizeof(str)/sizeof(str[0]), L"%d/%d pg.", m_curPg, m_numPg );
    if ( f.CreateFontIndirect( &lf ) )
    {
      HFONT olfF = dc.SelectFont( f );
      int oldBkMode = dc.SetBkMode( TRANSPARENT );
      COLORREF oldCol = dc.SetTextColor( RGB(0,0,0) ); // TODO: set "themed" color
      
      dc.DrawTextW( str, -1, &r, DT_LEFT|DT_TOP|DT_NOPREFIX|DT_WORDBREAK );
      
      dc.SetTextColor( oldCol );
      dc.SetBkMode( oldBkMode );
      dc.SelectFont( olfF );
    }
  }
  //dc.SelectPen( hOldPen );

  return 0;
}

LRESULT CDjVuToolBar::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled  = false;
  
  WTL::CRect r;
  if ( !GetOutRect( &r ) )
  {
    return 0;
  }

  WTL::CPoint p( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
  if ( r.PtInRect( p ) )
  {
    ::PostMessage( m_hWndFrame, WM_COMMAND, ID_NAVIGATION_GOTOPAGE, 0 ); 
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