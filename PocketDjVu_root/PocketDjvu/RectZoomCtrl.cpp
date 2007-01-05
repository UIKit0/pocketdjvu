#include "StdAfx.h"

#include <algorithm>

#include "./resource.h"

#include "./RectZoomCtrl.h"

LRESULT CRectZoomCtrl::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  p1.x = LOWORD(lParam); 
  p1.y = HIWORD(lParam);

  SetCapture( m_hWnd );

  return 0;
}

static void NormalizeRect( CRect & r )
{
  if ( r.right > r.left )
  {
    std::swap( r.right, r.left );
  }

  if ( r.bottom > r.top )
  {
    std::swap( r.bottom, r.top );
  }
}

LRESULT CRectZoomCtrl::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
{ 
  bHandled = true;
  ReleaseCapture();

  m_b2nd = false;
  CSize sz0( 0, 0 );
  CRect curR( p1, p2 );
  NormalizeRect( curR );

  CDC cdc( ::GetWindowDC( m_hWnd ) );
  cdc.DrawDragRect( curR, sz0, 0, sz0 );

  m_pSubscriber->FinishCtrl( this, false );
  return 0;
}

LRESULT CRectZoomCtrl::OnMouseMove( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  CRect prR( p1, p2 );
  NormalizeRect( prR );
  RECT * pPrevRect = 0;
  if ( m_b2nd )
  {
    pPrevRect = &prR;
  }
  p2.x = LOWORD(lParam);
  p2.y = HIWORD(lParam);

  CRect curR( p1, p2 );
  NormalizeRect( curR );

  CDC cdc( ::GetWindowDC( m_hWnd ) );
  CSize sz0( 0, 0 );
  cdc.DrawDragRect( curR, sz0, pPrevRect, sz0 );

  m_b2nd = true;
  return 0;
}

