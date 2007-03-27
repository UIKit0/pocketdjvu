// BookmarkDlg.cpp : Implementation of CBookmarkDlg

#include "stdafx.h"

#include "./Constants.h"
#include "./BookmarkDlg.h"
#include "./misc.h"

CBookmarkDlg::CBookmarkDlg( wchar_t const * szFullPathName, CBookmarkInfo const & bookmarkInfo ) :
  m_szCurFullPathName(szFullPathName)
  , m_curBookmarkInfo(bookmarkInfo)
  , m_bNotSaved()
{
  WTL::CString regPath( APP_REG_PATH );
  regPath += L"\\Bookmarks";
  m_rootReg.Create( HKEY_CURRENT_USER, regPath );
  ATLASSERT( m_rootReg.m_hKey );

  if ( !m_szCurFullPathName.IsEmpty() )
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

CBookmarkDlg::~CBookmarkDlg()
{
}	

LRESULT CBookmarkDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  m_hWndMenuBar = ::CreateDlgMenuBar( IDR_MENU_SAVE_GOTO, m_hWnd );
  
  Base::OnInitDialog( uMsg, wParam, lParam, bHandled );

  DoDataExchange();
  
  if ( m_szCurFullPathName.IsEmpty() )
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

void CBookmarkDlg::LoadBookmark( WTL::CString szPathKey )
{
  ATL::CRegKey pathKey;
  LONG res = pathKey.Open( m_rootReg, szPathKey );
  if ( ERROR_SUCCESS != res )
  {
    return;
  }
  szPathKey.Replace( '/', '\\' );

  WTL::CTreeItem pathItem;

  res = ERROR_SUCCESS;
  WTL::CString bookmarkName;
  wchar_t * buf = bookmarkName.GetBufferSetLength( MAX_PATH+1 );
  
  for( int i=0; ERROR_NO_MORE_ITEMS!=res && i<1024; ++i )
  {
    DWORD l = bookmarkName.GetAllocLength();
    LONG res = pathKey.EnumKey( i, buf, &l );
    if( ERROR_MORE_DATA == res )
    {
      buf = bookmarkName.GetBufferSetLength( l+1 );
      --i;
      continue;
    }
    if ( ERROR_SUCCESS != res )
    {
      continue;
    }    
    
    CBookmarkInfo bm;
    if ( !bm.LoadFromReg( pathKey, buf ) )
    {
      continue;
    }

    BMPtr pBM( new CBookmarkInfoRef( bm, buf ) );
    m_bms[ szPathKey ].insert( pBM );
    
    
    if ( pathItem.IsNull() )
    {
      pathItem = m_tree.InsertItem( szPathKey, 0, 0 );
      if ( pathItem.IsNull() )
      {
        return;
      }
    }
    WTL::CTreeItem bmItem = pathItem.AddTail( buf, 0 );
    bmItem.SetData( (DWORD_PTR)pBM.GetPtr() );
  }
}

void CBookmarkDlg::LoadFromRegistry()
{
  LONG res = ERROR_SUCCESS;
  WTL::CString keyName;
  wchar_t * buf = keyName.GetBufferSetLength( MAX_PATH+1 );  

  for( int i=0; ERROR_NO_MORE_ITEMS!=res && i<1024; ++i )
  {
    DWORD l = keyName.GetAllocLength();
    LONG res = m_rootReg.EnumKey( i, buf, &l );
    if( ERROR_MORE_DATA == res )
    {
      buf = keyName.GetBufferSetLength( l+1 );
      --i;
      continue;
    }
    if ( ERROR_SUCCESS != res )
    {
      continue;
    }    
    LoadBookmark( buf );
  }
}

void CBookmarkDlg::SaveToRegistry()
{  
  for ( BMs::iterator i=m_bms.begin(); i!=m_bms.end(); ++i )
  {
    WTL::CString keyPathName = i->first;
    keyPathName.Replace( '\\', '/' );
    // remove old values.
    m_rootReg.RecurseDeleteKey( keyPathName );

    // write new values.
    ATL::CRegKey pathReg;
    pathReg.Create( m_rootReg, keyPathName );
    if ( pathReg )
    {      
      for ( BMSet::iterator j=i->second.begin(); j!=i->second.end(); ++j )
      {
        CBookmarkInfoRef const & bmi = **j;
        bmi.SaveToReg( pathReg, bmi.GetName() );
      }
    }
  }
}

void CBookmarkDlg::FindOrCreateCurrentFileBranch()
{
  if ( m_szCurFullPathName.IsEmpty() )
    return;

  WTL::CTreeItem rootItem = m_tree.GetRootItem();
  if ( rootItem.IsNull() )
  {
    m_currentFileItem = m_tree.InsertItem( m_szCurFullPathName, 0, 0 );
    m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
    m_currentFileItem.Select();
    m_currentFileItem.Expand();    
    return;
  }
  
  for ( ; rootItem.IsNull(); rootItem = rootItem.GetNextSibling() )
  {
    wchar_t str[ MAX_PATH ] = {0};
    if ( rootItem.GetText( str, MAX_PATH ) && m_szCurFullPathName == str )
    {
      m_currentFileItem = rootItem;
      m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
      return;
    }
  }

  ATLASSERT( false );
  return;
}

LRESULT CBookmarkDlg::OnCancell( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  EndDialog( wID );
  return 0;
}

LRESULT CBookmarkDlg::OnSave( WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL& bHandled )
{
  if ( m_bNotSaved )
  {
    SaveToRegistry();
  }

  EndDialog( wID );
  return 0;
}

LRESULT CBookmarkDlg::OmGotoBookmark( WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL& bHandled )
{
  if ( m_bNotSaved )
  {
    SaveToRegistry();
  }

  // TODO: Save the info for selected bookmark...

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

  BMPtr pBM( new CBookmarkInfoRef( m_curBookmarkInfo, m_sBookmarkName ) );
  m_bms[ m_szCurFullPathName ].insert( pBM );
  ti.SetData( (DWORD_PTR)pBM.GetPtr() );

  m_bNotSaved = true;

  return 0;
}

LRESULT CBookmarkDlg::OnBtnDel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  WTL::CTreeItem ti = m_tree.GetSelectedItem();
  if ( ti.IsNull() )
    return 0;

  WTL::CTreeItem parent = ti.GetParent();
  if ( parent.IsNull() )
    return 0;
  WTL::CString path;
  parent.GetText( path );
  
  WTL::CTreeItem act = ti.GetNextSibling();
  if ( act.IsNull() )
  {
    act = ti.GetPrevSibling();
    if ( act.IsNull() )
    {
      act = parent;
    }
  }  

  BMPtr pBM( (CBookmarkInfoRef*)ti.GetData() );
  m_bms[ path ].erase( pBM );
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
  
  m_bNotSaved = true;

  return 0;
}

LRESULT CBookmarkDlg::OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  DoSipInfo();
  return 0;
}

void CBookmarkDlg::EnableGotoBookmarkMenu( bool bEnable )
{
  if ( !m_hWndMenuBar )
  {
    return;
  }
  // it's absent on PPC2003SE: SHEnableSoftkey( m_hWndMenuBar, ID_GOTOBOOKMARK, FALSE, bEnable );
  TBBUTTONINFO tbbi;
  tbbi.cbSize  = sizeof(tbbi);
  tbbi.dwMask  = TBIF_STATE;
  tbbi.fsState = bEnable ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE;

  ::SendMessage( m_hWndMenuBar, TB_SETBUTTONINFO, ID_GOTOBOOKMARK, (LPARAM)&tbbi );
}

LRESULT CBookmarkDlg::OnTvnSelchangedTree( int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& bHandled )
{
  WTL::CTreeItem it( reinterpret_cast<LPNMTREEVIEW>(pNMHDR)->itemNew.hItem, &m_tree );
  WTL::CTreeItem parent = it.GetParent();

  if ( parent.IsNull() )
  {
    EnableGotoBookmarkMenu( false );
    return 0;
  }
  
  EnableGotoBookmarkMenu( true );
  it.GetText( m_sBookmarkName );
  DoDataExchange( FALSE, IDC_BOOKMARK_NAME );
 
  parent.GetText( m_szSelectedFullPathName );

  return 0;
}
