#include "StdAfx.h"

#include "ScrollByTap.h"

LRESULT CScrollByTap::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  CPoint p( lParam );

  CRect rc;
  if ( !::GetClientRect( m_hWnd, &rc ) )
  {
    m_pSubscriber->FinishCtrl( this, true );
    return 0;
  }

  SetCapture( m_hWnd );

  CPoint c = rc.CenterPoint();
  float x = float(p.x - c.x) * 2 / rc.Width();
  float y = float(p.y - c.y) * 2 / rc.Height();
  float r = (float)sqrt( x*x + y*y );
  x /=  r;
  y /= -r;

  float const sin45 = 0.70710678118654752440084436210485f;
  if( 0 <= x && -sin45 <= y && y <= sin45 )
  {
    // right
    m_vkey = VK_RIGHT;
  }
  else if( x <= 0 && -sin45 <= y && y <= sin45 )
  {
    // left
    m_vkey = VK_LEFT;
  }
  else if( 0 <= y && -sin45 <= x && x <= sin45 )
  {
    // up
    m_vkey = VK_UP;
  }
  else
  {
    // down
    m_vkey = VK_DOWN;
  }

  ::PostMessage( m_hWnd, WM_KEYDOWN, m_vkey, 0 );

  return 0;
}

LRESULT CScrollByTap::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{
  bHandled = true;
  ReleaseCapture();

  ::PostMessage( m_hWnd, WM_KEYUP, m_vkey, 0 );

  return 0;
}