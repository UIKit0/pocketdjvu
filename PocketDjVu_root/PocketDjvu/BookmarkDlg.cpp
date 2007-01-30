// BookmarkDlg.cpp : Implementation of CBookmarkDlg

#include "stdafx.h"
#include "Constants.h"

#include "BookmarkDlg.h"

CBookmarkDlg::CBookmarkDlg( wchar_t const * szFullPathName, CBookmarkInfo const & bookmarkInfo ) :
  m_szFullPathName(szFullPathName)
  , m_bookmarkInfo(bookmarkInfo)
{
  WTL::CString regPath( APP_REG_PATH );
  regPath += L"\\Bookmarks";
  m_rootReg.Create( HKEY_CURRENT_USER, regPath );
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

    l = GetDateFormat( NULL, DATE_SHORTDATE, &st, NULL, NULL, 0 );
    if ( l )
    {
      l = GetDateFormat( NULL, DATE_SHORTDATE, &st, NULL, sDate.GetBufferSetLength(l), l );
      sDate.ReleaseBuffer();
    }
    m_sBookmarkName = sTime + L" - " + sDate;
  }
}

BOOL CBookmarkDlg::CreateMenuBar( UINT nToolBarId )
{
  SHMENUBARINFO mbi = { 0 };
  mbi.cbSize      = sizeof(mbi);
  mbi.hwndParent  = m_hWnd;
  mbi.dwFlags     = SHCMBF_HMENU;
  mbi.nToolBarId  = nToolBarId;
  mbi.hInstRes    = GetModuleHandle( NULL );

  BOOL bRet = ::SHCreateMenuBar( &mbi );
  if( bRet )
  {
    HWND m_hWndMnuBar = mbi.hwndMB;
  }

  return bRet;
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

  CreateMenuBar( IDR_MENU_OKCANCEL );
    
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
    m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
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
      m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
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

LRESULT CBookmarkDlg::OnBtnAdd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  if ( m_currentFileItem.IsNull() )
    return 0;

  DoDataExchange( TRUE, IDC_BOOKMARK_NAME );

  WTL::CTreeItem ti = m_currentFileItem.AddTail( m_sBookmarkName, 0 );
  m_currentFileItem.SortChildren( FALSE );
  m_currentFileItem.Expand();
  ti.Select();
  m_tree.EnsureVisible( ti );

  bHandled = true;
  return 0;
}

LRESULT CBookmarkDlg::OnBtnDel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  bHandled = true;
  WTL::CTreeItem ti = m_tree.GetSelectedItem();
  if ( ti.IsNull() )
    return 0;

  WTL::CTreeItem parent = ti.GetParent();
  if ( parent.IsNull() )
    return 0;
  
  WTL::CTreeItem act = ti.GetNextSibling();
  if ( act.IsNull() )
  {
    act = ti.GetPrevSibling();
    if ( act.IsNull() )
    {
      act = parent;
    }
  }

  ti.Delete();

  if ( parent != m_currentFileItem && !parent.HasChildren() )
  {
    act = parent.GetNextSibling();
    if ( act.IsNull() )
    {
      act = parent.GetPrevSibling();
    }
    parent.Delete();
  }
  
  if ( !act.IsNull() )
  {
    act.Select();
    m_tree.EnsureVisible( act );
  }

  return 0;
}

LRESULT CBookmarkDlg::OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;
  DoSipInfo();
  return 0;
}

LRESULT CBookmarkDlg::OnTvnSelchangedTree( int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled )
{
  WTL::CTreeItem it( reinterpret_cast<LPNMTREEVIEW>(pNMHDR)->itemNew.hItem, &m_tree );

  if ( !it.GetParent().IsNull() )
  {
    it.GetText( m_sBookmarkName );
    DoDataExchange( FALSE, IDC_BOOKMARK_NAME );
  }

  bHandled = true;
  return 0;
}
