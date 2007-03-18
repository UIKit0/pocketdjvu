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
    : m_hWndSIP(), m_curPg(), m_numPg(), m_bPostHistoryCommand()
  {
  }

  void SetFrameWnd( HWND hWndFrame )
  {
    m_hWndFrame = hWndFrame;
  }

  void SetPages( int curPg=0, int numPg=0 );

  BEGIN_MSG_MAP(CDjVuToolBar)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
  END_MSG_MAP()

  LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  bool SubclassToolbar( HWND hWndMenuBar );

  POINT GetPointForMenu();

private:
  enum BTN_ZONE
  {
    NOPE,
    BACK,
    FORWARD,
    PANE
  };
  enum BTN_ZONE TestBtnZone( WTL::CPoint p, WTL::CRect const & panelRect ) const;
  bool GetOutRect( RECT * rect );
  void DrawHistoryButtond( WTL::CDC & dc, WTL::CRect & panelRect );
  bool DrawArrow( WTL::CDC & dc, WTL::CBitmap const & bmp, WTL::CRect r );

private:
  HWND m_hWndSIP;
  HWND m_hWndFrame;
  int m_curPg;
  int m_numPg;
  bool m_bPostHistoryCommand;

  WTL::CBitmap m_back;
  WTL::CBitmap m_forward;
};
