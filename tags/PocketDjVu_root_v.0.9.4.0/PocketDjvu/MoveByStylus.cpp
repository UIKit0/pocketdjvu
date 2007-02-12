#include "StdAfx.h"

#include "Constants.h"
#include "MoveByStylus.h"

LRESULT CMoveByStylus::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  
  DWORD t = GetTickCount();
  if ( t - m_1stClick < g_cDobleClickTime )
  {
    m_pSubscriber->FinishCtrl( this, true );
    return 0;
  }
  m_1stClick = t;

  m_p1.x = LOWORD(lParam); 
  m_p1.y = HIWORD(lParam);

  SetCapture( m_hWnd );  

  return 0;
}

LRESULT CMoveByStylus::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
  bHandled = true;
  
  ReleaseCapture();
  m_mainFrame.PageLayout();
  m_mainFrame.Invalidate();

  return 0;
}

LRESULT CMoveByStylus::OnMouseMove( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;

  WTL::CPoint p2( lParam );
  WTL::CPoint v = p2 - m_p1;
  m_mainFrame.MoveImage( v, 0 );
  m_p1 = p2;
  
  if ( g_sDxDblClck <= abs(v.x) || g_sDxDblClck <= abs(v.y) )
  {
    m_1stClick = 0;
  }

  return 0;
}
