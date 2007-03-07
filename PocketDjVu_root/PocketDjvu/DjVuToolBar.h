#pragma once

namespace siv_hlpr_tb
{
  class CImpWin : public ATL::CWindowImplBase
  {
  public:
    CImpWin( HWND )
    {
    }
  };
} //namespace siv_hlpr_tb

class CDjVuToolBar : public WTL::CToolBarCtrlT< siv_hlpr_tb::CImpWin >
{
  typedef WTL::CToolBarCtrlT< siv_hlpr_tb::CImpWin > Base;
public:
  CDjVuToolBar()
    : m_hWndSIP(), m_curPg(), m_numPg()
  {
  }

  void SetFrameWnd( HWND hWndFrame )
  {
    m_hWndFrame = hWndFrame;
  }

  void SetPages( int curPg=0, int numPg=0 );

  BEGIN_MSG_MAP(CDjVuToolBar)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
  END_MSG_MAP()

  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  bool SubclassToolbar( HWND hWndMenuBar );

private:
  bool GetOutRect( RECT * rect );

private:
  HWND m_hWndSIP;
  HWND m_hWndFrame;
  int m_curPg;
  int m_numPg;
};
