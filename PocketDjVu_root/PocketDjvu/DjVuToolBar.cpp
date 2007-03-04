#include "StdAfx.h"

#include "./resource.h"
#include "./DjVuToolBar.h"
#include "./misc.h"
#include "./constants.h"

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

LRESULT CDjVuToolBar::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  DefWindowProc();

  WTL::CDC dc = GetDC();

  WTL::CRect sipR;
  if ( !::GetWindowRect( m_hWndSIP, sipR ) )
  {
    return 0;
  }
  ScreenToClient( (POINT*)&sipR );
  ScreenToClient( 1+(POINT*)&sipR );

  WTL::CRect r;
  if ( !GetRect( m_theLastButtonID, &r ) )
  {
    return 0;
  }
  
  // TODO: transfer the rect calculation into separate method
  //      check the SIP button position and don't draw our panel if it is in the middle position (WM 5.0)

  r.left  = r.right + g_cTollBarGap;
  r.right = sipR.left - g_cTollBarGap;
   
  WTL::CRect winR;
  GetWindowRect( &winR );

  r.top     = 0;
  r.bottom  = sipR.bottom;
  
  //WTL::CPen pen( (HPEN)GetStockObject(BLACK_PEN) );

  //HPEN hOldPen = dc.SelectPen( pen );
  {
    WTL::CFont f;
    LOGFONT lf = {0};
    lf.lfHeight = -14;
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    
    dc.DrawEdge( &r, BDR_RAISEDINNER, BF_RECT|BF_MONO );

    r.DeflateRect( 3, 3 );
    if ( f.CreateFontIndirect( &lf ) )
    {
      HFONT olfF = dc.SelectFont( f );
      dc.SetBkMode( TRANSPARENT );

      dc.DrawTextW( L"123 / 1002", -1, &r, DT_LEFT|DT_TOP|DT_NOPREFIX|DT_WORDBREAK );
      dc.SelectFont( olfF );
    }
  }
  //dc.SelectPen( hOldPen );

  return 0;
}
