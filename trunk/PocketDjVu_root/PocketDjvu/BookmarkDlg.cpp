// BookmarkDlg.cpp : Implementation of CBookmarkDlg

#include "stdafx.h"
#include "BookmarkDlg.h"

CBookmarkDlg::CBookmarkDlg( bool bAddBookmark ) : 
  m_bAddBookmark(bAddBookmark)
{
}

CBookmarkDlg::~CBookmarkDlg()
{
}	

LRESULT CBookmarkDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  Base::OnInitDialog( uMsg, wParam, lParam, bHandled );
  bHandled = true;
  return 1;  // Let the system set the focus
}

LRESULT CBookmarkDlg::OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  EndDialog( wID );
  return 0;
}

LRESULT CBookmarkDlg::OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  DoSipInfo();
  return 0;
}