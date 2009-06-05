#pragma once

#include "./ICtrlNotify.h"

class CRectZoomCtrl : public CControllerBase
{
public:
  CRectZoomCtrl( ICtrlNotify * pSubscriber ) : CControllerBase(pSubscriber)
    , m_b2nd()
  {
  }
  virtual ~CRectZoomCtrl()
  {
  }

  WTL::CRect GetRect()
  {
    return WTL::CRect( p1, p2 );
  }

public:
  BEGIN_MSG_MAP(CRectZoomCtrl)
    CHAIN_MSG_MAP(CControllerBase)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
    MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
  END_MSG_MAP()

public:
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  WTL::CPoint p1;
  WTL::CPoint p2;
  bool m_b2nd;  
};
