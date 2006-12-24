#pragma once

#include "./ICtrlNotify.h"

#include "smart_ptr.h"
//=============================================================================
class CRectZoomCtrl :
  public siv_hlpr::CRefCntr<>
  , public CMessageMap
{
public:
  CRectZoomCtrl( ICtrlNotify * pSubscriber );
  ~CRectZoomCtrl();

  CRect GetRect()
  {
    return CRect( p1, p2 );
  }

public:
  BEGIN_MSG_MAP(CRectZoomCtrl)
    m_hWnd = hWnd; // SIV: small hack to get HWND without overriding
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
    MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
  END_MSG_MAP()

public:
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  HWND m_hWnd;
  ICtrlNotify * m_pSubscriber;
  CPoint p1;
  CPoint p2;
  bool m_b2nd;  
};

typedef siv_hlpr::CSimpSPtr<CRectZoomCtrl> RectZoomCtrlPtr;
