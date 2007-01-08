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
    CString s;
    unsigned const maxL = 128;
    verWnd.GetWindowText( s.GetBuffer(maxL), maxL );
    s.ReleaseBuffer();
    CString verStr = GetModuleVersionStr( 0 );
    s += verStr;
    verWnd.SetWindowText( s );
  }

  return TRUE;
}
