// BookmarkDlg.cpp : Implementation of CBookmarkDlg

#include "stdafx.h"

#include "./Values.h"
#include "./BookmarkDlg.h"
#include "./misc.h"

UINT CBookmarkDlg::m_BookmarkDlgMsg = ::RegisterWindowMessage( L"730ABCF1-492F-49f7-A11A-9C3A143E47CB" );

CBookmarkDlg::CBookmarkDlg( wchar_t const * szFullPathName, CBookmarkInfo const & bookmarkInfo ) :
  m_szCurFullPathName(szFullPathName)
  , m_curBookmarkInfo(bookmarkInfo)
  , m_bNotSaved()
{
  WTL::CString regPath( APP_REG_PATH );
  regPath += BOOKMARK_REG_KEY;
  m_rootReg.Create( HKEY_CURRENT_USER, regPath );
  ATLASSERT( m_rootReg.m_hKey );

  if ( !m_szCurFullPathName.IsEmpty() )
  {
    WTL::CString sTime, sDate;
    SYSTEMTIME st;
    GetLocalTime( &st );
    wchar_t const * szTimeFmt = L"HH:mm:ss";
    int l = GetTimeFormat( NULL, 0, &st, szTimeFmt, NULL, 0 );
    if ( l )
    {    
      l = GetTimeFormat( NULL, 0, &st, szTimeFmt, sTime.GetBufferSetLength(l), l );
      sTime.ReleaseBuffer();
    }

    wchar_t const * szFateFmt = L"yyyy.MM.dd";
    l = GetDateFormat( NULL, 0, &st, szFateFmt, NULL, 0 );
    if ( l )
    {
      l = GetDateFormat( NULL, 0, &st, szFateFmt, sDate.GetBufferSetLength(l), l );
      sDate.ReleaseBuffer();
    }
    m_sBookmarkName = sDate + L" - " + sTime;
  }
}

CBookmarkDlg::~CBookmarkDlg()
{
}	

LRESULT CBookmarkDlg::OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  m_hWndMenuBar = ::CreateDlgMenuBar( IDR_MENU_SAVE_GOTO, m_hWnd );
  
  Base::OnInitDialog( uMsg, wParam, lParam, bHandled );
  bHandled = true;

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

  WTL::CString bookmarkName;
  wchar_t * buf = bookmarkName.GetBufferSetLength( MAX_PATH+1 );
  res = ERROR_SUCCESS;  
  for( int i=0; ERROR_NO_MORE_ITEMS!=res && i<1024; ++i )
  {
    DWORD l = bookmarkName.GetAllocLength();
    res = pathKey.EnumKey( i, buf, &l );
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
      pathItem = m_tree.InsertItem( szPathKey, 0, TVI_SORT );
      if ( pathItem.IsNull() )
      {
        return;
      }
    }
    WTL::CTreeItem bmItem = pathItem.AddTail( buf, 0 );
    pathItem.SortChildren();
    bmItem.SetData( (DWORD_PTR)pBM.GetPtr() );
  }
}

void CBookmarkDlg::LoadFromRegistry()
{  
  WTL::CString keyName;
  wchar_t * buf = keyName.GetBufferSetLength( MAX_PATH+1 );  

  LONG res = ERROR_SUCCESS;
  for( int i=0; ERROR_NO_MORE_ITEMS!=res && i<1024; ++i )
  {
    DWORD l = keyName.GetAllocLength();
    res = m_rootReg.EnumKey( i, buf, &l );
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

bool CBookmarkDlg::SaveAutoBM( wchar_t const * szFullPath, CBookmarkInfo const & rBM )
{
  WTL::CString keyFileKey= szFullPath;
  keyFileKey.Replace( '\\', '/' );

  WTL::CString regPath( APP_REG_PATH );  
  regPath += BOOKMARK_REG_KEY;
  regPath += L"\\";
  regPath += keyFileKey;

  ATL::CRegKey reg;
  reg.Create( HKEY_CURRENT_USER, regPath );
  if ( !reg )
  {
    return false;
  }
  return rBM.SaveToReg( reg, BOOKMARK_REG_AUTOSAVE );
}

bool CBookmarkDlg::LoadAutoBM( wchar_t const * szFullPath, CBookmarkInfo & rBM )
{
  WTL::CString keyFileKey = szFullPath;
  keyFileKey.Replace( '\\', '/' );

  WTL::CString regPath( APP_REG_PATH );
  regPath += BOOKMARK_REG_KEY;
  regPath += L"\\";
  regPath += keyFileKey;

  ATL::CRegKey reg;
  reg.Open( HKEY_CURRENT_USER, regPath );
  if ( !reg )
  {
    return false;
  }
  return rBM.LoadFromReg( reg, BOOKMARK_REG_AUTOSAVE );
}

void CBookmarkDlg::DoesAutoBMExistOnly( wchar_t const * szFullPath )
{
  if ( !szFullPath || !szFullPath[0] )
  {
    return;
  }

  WTL::CString keyFileKey= szFullPath;
  keyFileKey.Replace( '\\', '/' );

  WTL::CString regPath( APP_REG_PATH );  
  regPath += BOOKMARK_REG_KEY;
  regPath += L"\\";
  regPath += keyFileKey;

  ATL::CRegKey reg;
  reg.Open( HKEY_CURRENT_USER, regPath );
  if ( !reg )
  {
    return;
  }

  DWORD subKeyNum = ~0;
  if ( ERROR_SUCCESS != ::RegQueryInfoKey( reg, 0, 0, 0, &subKeyNum, 0, 0, 0, 0, 0, 0, 0 )
       || 1 != subKeyNum
     )
  {
    return;
  }

  reg.Close();
  WTL::CString regAutoPath = regPath + L"\\" + BOOKMARK_REG_AUTOSAVE;
  reg.Open( HKEY_CURRENT_USER, regAutoPath );
  if ( !reg )
  {
    return;
  }
  reg.Close();

  regPath = APP_REG_PATH;
  regPath += BOOKMARK_REG_KEY;
  reg.Open( HKEY_CURRENT_USER, regPath );
  if ( !reg )
  {
    return;
  }
  reg.DeleteSubKey( keyFileKey );
}

void CBookmarkDlg::SaveToRegistry()
{
  for ( BMs::iterator i=m_bms.begin(); i!=m_bms.end(); ++i )
  {
    WTL::CString keyPathName = i->first;
    keyPathName.Replace( '\\', '/' );
    // remove old values.
    LONG res = m_rootReg.DeleteSubKey( keyPathName );
    ATLASSERT( ERROR_SUCCESS == res );
    {
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
}

void CBookmarkDlg::FindOrCreateCurrentFileBranch()
{
  if ( m_szCurFullPathName.IsEmpty() )
    return;
  
  for ( WTL::CTreeItem rootItem = m_tree.GetRootItem();
        !rootItem.IsNull();
        rootItem = rootItem.GetNextSibling() )
  {
    wchar_t str[ MAX_PATH+1 ] = {0};
    if ( rootItem.GetText( str, MAX_PATH ) && m_szCurFullPathName == str )
    {
      m_currentFileItem = rootItem;
      m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
      return;
    }
  }

  m_currentFileItem = m_tree.InsertItem( m_szCurFullPathName, 0, 0 );
  m_currentFileItem.SetState( TVIS_BOLD, TVIS_BOLD );
  m_currentFileItem.Select();
  m_currentFileItem.Expand();
  return;
}

LRESULT CBookmarkDlg::OnCancell( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled )
{
  EndDialog( IDCANCEL );
  return 0;
}

LRESULT CBookmarkDlg::OnSave( WORD wNotifyCode,WORD wID,HWND hWndCtl,BOOL& bHandled )
{
  // If it's not the [Ok] button in the upper right conner we got this 
  // notification probably from edit box during editing tree item.
  if ( hWndCtl )
  {
    return 0;
  }

  if ( m_bNotSaved )
  {
    SaveToRegistry();
  }

  EndDialog( wID );
  return 0;
}

LRESULT CBookmarkDlg::OnGotoBookmark( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
  if ( m_bNotSaved )
  {
    SaveToRegistry();
  }

  WTL::CTreeItem selIt = m_tree.GetSelectedItem();
  if ( selIt.IsNull() || selIt.GetParent().IsNull() )
  {
    EndDialog( IDCANCEL );
    return 0;
  }
  selIt.GetParent().GetText( m_szCurFullPathName );
  m_pGoToBM = BMPtr( (CBookmarkInfoRef*)selIt.GetData() );  
  EndDialog( m_pGoToBM ? ID_GOTOBOOKMARK : IDCANCEL );
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
  BMPtr pBM( (CBookmarkInfoRef*)ti.GetData() );
  m_bms[ path ].erase( pBM );
  ti.Delete();
  
  WTL::CTreeItem act = ti.GetNextSibling();
  if ( act.IsNull() )
  {
    act = ti.GetPrevSibling();
    if ( act.IsNull() )
    {
      act = parent;
    }
  }  

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

  return 0;
}

LRESULT CBookmarkDlg::OnDblclkTree( int /*idCtrl*/, LPNMHDR /*pNMHDR*/, BOOL& bHandled )
{
  WTL::CPoint p;
  GetCursorPos( &p );
  m_tree.InsertItem(0);

  WTL::CTreeItem it = m_tree.GetSelectedItem();
  if ( it.IsNull() || it.GetParent().IsNull() )
  {
    bHandled = false;
    return 0;
  }
  OnGotoBookmark( 0, ID_GOTOBOOKMARK, 0, bHandled );

  return 0;
}

LRESULT CBookmarkDlg::OnTvnBeginLabelEditTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);  
  WTL::CTreeItem it( pTVDispInfo->item.hItem, &m_tree );
  if ( it.GetParent().IsNull() )
  {
    return TRUE; // Cancell editing
  }

  return FALSE;
}

LRESULT CBookmarkDlg::OnTvnEndLabelEditTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);  
  if ( !pTVDispInfo->item.pszText )
  {
    return FALSE; // reject the edited text and revert to the original label
  }

  WTL::CTreeItem it( pTVDispInfo->item.hItem, &m_tree );
  WTL::CTreeItem parent = it.GetParent();
  if ( parent.IsNull() )
  {
    return FALSE; // reject the edited text and revert to the original label
  }

  BMPtr p( (CBookmarkInfoRef*)it.GetData() );
  if ( !p )
  {
    return FALSE; // reject the edited text and revert to the original label
  }
  p->SetName( pTVDispInfo->item.pszText );

  PostMessage( m_BookmarkDlgMsg, MSG_SORTandSHOW, LPARAM(it.m_hTreeItem) );  

  m_sBookmarkName = pTVDispInfo->item.pszText;
  DoDataExchange( FALSE, IDC_BOOKMARK_NAME );
  m_bNotSaved = true;

  return TRUE;
}

LRESULT CBookmarkDlg::OnBookmarkDlgMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  switch ( wParam ) 
  {
    case MSG_SORTandSHOW:
    {
      WTL::CTreeItem it( HTREEITEM(lParam), &m_tree );
      WTL::CTreeItem parent = it.GetParent();
      if ( !parent.IsNull() )
      {
        parent.SortChildren();
        it.EnsureVisible();
      }
    }
  }
  return 0;
}
