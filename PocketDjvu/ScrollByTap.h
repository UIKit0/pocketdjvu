#pragma once

#include "./ICtrlNotify.h"

class CScrollByTap : public CControllerBase
{
public:
  CScrollByTap( ICtrlNotify * pSubscriber ) : CControllerBase(pSubscriber)
    , m_vkey()
  {
  }
  virtual ~CScrollByTap()
  {
  }

public:
  BEGIN_MSG_MAP(CRectZoomCtrl)
    CHAIN_MSG_MAP(CControllerBase)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)    
  END_MSG_MAP()

public:
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

// DATA:
private:
  WPARAM m_vkey;
};
