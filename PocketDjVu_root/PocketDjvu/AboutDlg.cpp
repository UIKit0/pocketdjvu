// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "./misc.h"


LRESULT CAboutDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  Base::OnInitDialog( uMsg, wParam, lParam, bHandled );
  bHandled = true;

  CWindow verWnd = GetDlgItem(IDC_STATIC_VER);
  if ( verWnd.IsWindow() )
  {
    WTL::CString s;
    unsigned const maxL = 128;
    verWnd.GetWindowText( s.GetBuffer(maxL), maxL );
    s.ReleaseBuffer();
    WTL::CString verStr = GetModuleVersionStr( 0 );
    s += verStr;
    verWnd.SetWindowText( s );
  }

  //
  MEMORYSTATUS ms = {0};
  ms.dwLength = sizeof ms;
  GlobalMemoryStatus( &ms );
  CWindow siWnd = GetDlgItem(IDC_INFO);
  if ( siWnd.IsWindow() )
  {
    WTL::CString s;
    PROCESSOR_ARCHITECTURE_ARM;
    s.Format( L"TotalPhys : %d\r\nAvailPhys : %d\r\nTotalVirtual : %d\r\nAvailVirtual : %d\r\n",
              int(ms.dwTotalPhys),
              int(ms.dwAvailPhys),
              int(ms.dwTotalVirtual),
              int(ms.dwAvailVirtual)
            );
    siWnd.SetWindowText( s );
  }

  m_homePageLink.SubclassWindow( GetDlgItem(IDC_HOMEPAGE) );
  ATL::CString link;
  m_homePageLink.GetWindowText( link );
  m_homePageLink.SetHyperLink( link );
  m_homePageLink.SetHyperLinkExtendedStyle( HLINK_UNDERLINEHOVER, HLINK_UNDERLINEHOVER );

  ::CreateDlgMenuBar( IDR_MENU_OKCANCEL, m_hWnd );

  return TRUE;
}

LRESULT CAboutDlg::OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  DoSipInfo();
  return 0;
}