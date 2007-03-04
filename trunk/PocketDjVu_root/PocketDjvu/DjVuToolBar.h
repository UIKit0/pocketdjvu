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
  CDjVuToolBar() : m_hWndSIP(), m_theLastButtonID()
  {
  }

  void SetLastButtonID( int theLastButtonID )
  {
    m_theLastButtonID = theLastButtonID;
  }

  BEGIN_MSG_MAP(CDjVuToolBar)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    //CHAIN_MSG_MAP(Base)
  END_MSG_MAP()

  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  bool SubclassToolbar( HWND hWndMenuBar );

private:
  HWND m_hWndSIP;
  int m_theLastButtonID;
};
