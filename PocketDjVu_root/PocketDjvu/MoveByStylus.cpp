#include "StdAfx.h"

#include "MoveByStylus.h"

LRESULT CMoveByStylus::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  
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

  CPoint p2( lParam );
  CPoint v = p2 - m_p1;
  m_mainFrame.MoveImage( v, 0 );
  m_p1 = p2;

  return 0;
}
