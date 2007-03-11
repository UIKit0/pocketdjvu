#include "stdafx.h"
#include "resource.h"

#include "./GoToPage.h"
#include "./misc.h"

LRESULT CGoToPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  BOOL res = FALSE;
  if ( m_SIPstate.IsValid() )
  {
    WTL::CRect r = m_SIPstate.rcVisibleDesktop;
    r.bottom = m_SIPstate.rcSipRect.top;
    WTL::CPoint v1 = r.CenterPoint();
    res = GetWindowRect( &r );
    if ( res )
    {
      WTL::CPoint v2 = r.CenterPoint();
      v1 -= v2;
      ::SetWindowPos( m_hWnd, NULL, r.left + v1.x, r.top + v1.y, -1, -1,
			              SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
    }
  }
  if ( !res )
  {
    CenterWindow(GetParent());
  }

  DoDataExchange();

  m_UpDn = GetDlgItem(IDC_SPIN1);
  m_UpDn.SetRange( 1, m_maxPg );
  m_UpDn.SetPos( m_curPg );
  static UDACCEL accel[] = { {1,1}, {3,5}, {10,10} };
  m_UpDn.SetAccel( sizeof(accel)/sizeof(accel[0]), accel );
  
  //................................
  {
    CWindow wndTotal = GetDlgItem(IDC_TOTAL_PGS);
    WTL::CString sPgNum;
    sPgNum.Format( IDS_TOTAL_PGS, m_maxPg ); // L"/ %d pgs."    
    wndTotal.SetWindowText( sPgNum );
  }

  ::CreateDlgMenuBar( IDR_MENU_OKCANCEL, m_hWnd );

  return TRUE;
}

LRESULT CGoToPage::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if ( DoDataExchange( TRUE ) )
  {
    EndDialog(wID);
  }
  return 0;
}
