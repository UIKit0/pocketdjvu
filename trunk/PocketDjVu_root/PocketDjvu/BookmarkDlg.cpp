// BookmarkDlg.cpp : Implementation of CBookmarkDlg

#include "stdafx.h"
#include "Constants.h"

#include "BookmarkDlg.h"

CBookmarkDlg::CBookmarkDlg( wchar_t const * szFullPathName
                            , int pageIndex
                            , WTL::CRect const & pageRect
                            , bool bPortrait ) :
  m_szFullPathName(szFullPathName)
  , m_pageIndex(pageIndex)
  , m_pageRect(pageRect)
  , m_bPortrait(bPortrait)
{
  m_rootReg.Create( HKEY_CURRENT_USER, APP_REG_PATH );
  ATLASSERT( m_rootReg.m_hKey );

  if ( !m_szFullPathName.IsEmpty() )
  {
    WTL::CString sTime, sDate;
    SYSTEMTIME st;
    GetLocalTime( &st );
    int l = GetTimeFormat( NULL, 0, &st, NULL, NULL, 0 );
    if ( l )
    {    
      l = GetTimeFormat( NULL, 0, &st, NULL, sTime.GetBufferSetLength(l), l );
      sTime.ReleaseBuffer();
    }
    m_sBookmarkName = sTime + L" - " + sDate;
  }
}

CBookmarkDlg::~CBookmarkDlg()
{
}	

LRESULT CBookmarkDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  Base::OnInitDialog( uMsg, wParam, lParam, bHandled );
  bHandled = true;

  DoDataExchange();
  
  if ( m_szFullPathName.IsEmpty() )
  {
    ATL::CWindow wnd = GetDlgItem( IDC_BOOKMARK_NAME );
    if( wnd.IsWindow() )
    {
      wnd.EnableWindow( FALSE );
    }
    wnd = GetDlgItem( IDC_BTNADD );
    if( wnd.IsWindow() )
    {
      wnd.EnableWindow( FALSE );
    }
  }

  LoadFromRegistry();
  FindOrCreateCurrentFileBranch();
    
  return 1;  // Let the system set the focus
}

void CBookmarkDlg::LoadFromRegistry()
{
}

void CBookmarkDlg::FindOrCreateCurrentFileBranch()
{
  if ( m_szFullPathName.IsEmpty() )
    return;

  WTL::CTreeItem rootItem = m_tree.GetRootItem();
  if ( rootItem.IsNull() )
  {
    m_currentFileItem = m_tree.InsertItem( m_szFullPathName, 0, 0 );
    m_currentFileItem.Select();
    m_currentFileItem.Expand();
    return;
  }
  
  for ( ; rootItem.IsNull(); rootItem = rootItem.GetNextSibling() )
  {
    wchar_t str[ MAX_PATH ] = {0};
    if ( rootItem.GetText( str, MAX_PATH ) && m_szFullPathName == str )
    {
      m_currentFileItem = rootItem;
      return;
    }
  }

  ATLASSERT( false );
  return;
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