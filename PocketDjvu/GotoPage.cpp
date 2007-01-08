#include "stdafx.h"
#include "resource.h"

#include "GoToPage.h"

LRESULT CGoToPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_sipinfo.cbSize = sizeof m_sipinfo;
  BOOL res = SHSipInfo( SPI_GETSIPINFO, 0, &m_sipinfo, 0 );
  if ( res )
  {
    CRect r = m_sipinfo.rcVisibleDesktop;
    r.bottom = m_sipinfo.rcSipRect.top;
    CPoint v1 = r.CenterPoint();
    res = GetWindowRect( &r );
    if ( res )
    {
      CPoint v2 = r.CenterPoint();
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

 	return TRUE;
}

LRESULT CGoToPage::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  SHSipInfo( SPI_SETSIPINFO, 0, &m_sipinfo, 0 );
  DoDataExchange( TRUE );
	EndDialog(wID);
	return 0;
}
