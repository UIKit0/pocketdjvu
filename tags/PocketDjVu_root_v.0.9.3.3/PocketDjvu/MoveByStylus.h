#pragma once

#include "./ICtrlNotify.h"
#include "resource.h"
#include "MainFrm.h"

class CMoveByStylus : public CControllerBase
{
public:
  CMoveByStylus( CMainFrame * pSubscriber ) : CControllerBase(pSubscriber)
    , m_mainFrame( *pSubscriber )
    , m_1stClick()
  {
    ATLASSERT( m_mainFrame );
  }

  virtual ~CMoveByStylus()
  {
  }

public:
  BEGIN_MSG_MAP(CMoveByStylus)
    CHAIN_MSG_MAP(CControllerBase)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
    MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
  END_MSG_MAP()

public:
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  DWORD m_1stClick;
  CMainFrame & m_mainFrame;
  CPoint m_p1;
};
