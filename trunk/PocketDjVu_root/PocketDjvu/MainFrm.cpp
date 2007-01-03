// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../libdjvu/DjVuImage.h"
#include "../libdjvu/GBitmap.h"

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"
#include "PageLoader.h"
#include "GotoPage.h"

CMainFrame::CMainFrame() :
  m_bFullScreen(false)
  , m_curClientWidth()
  , m_initDir(L"\\")
  , m_appInfo( L"Software\\landi-soft.com\\PocketDjVu" )
  , m_scSate( SC_START )
  , m_timerID()
  , m_cursorKey()
  , m_bDirty()
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{  
  UIUpdateToolBar();
  AppSave();
  return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CreateSimpleCEMenuBar( IDR_MAINFRAME, SHCMBF_HMENU );

  HINSTANCE hinst = ATL::_AtlBaseModule.GetModuleInstance();

#pragma region Toolbar
  UINT nResourceID = IDR_MFRTB;

  HRSRC hRsrc = ::FindResource( hinst, MAKEINTRESOURCE(nResourceID), RT_TOOLBAR );
  HGLOBAL hGlobal = ::LoadResource( hinst, hRsrc );
  _AtlToolBarData * pData = (_AtlToolBarData*)::LockResource( hGlobal );
  ATLASSERT(pData->wVersion == 1);

  WORD * pItems = pData->items();
  int nItems = pData->wItemCount;

  TBBUTTON * pButtons = (TBBUTTON*)_alloca( nItems * sizeof(TBBUTTON) );
  ZeroMemory( pButtons, nItems * sizeof(TBBUTTON) );
  unsigned nIcon = 0;
  for( int i = 0; i < nItems; ++i )
  {
    if ( pItems[i] )
    {
      pButtons[i].iBitmap = i;
      pButtons[i].idCommand = pItems[i];
      pButtons[i].fsState = TBSTATE_ENABLED;
      pButtons[i].fsStyle = TBSTYLE_BUTTON;

      ++nIcon;
    }
    else
    {
      pButtons[i].fsStyle = TBSTYLE_SEP;
    }
  }
#pragma endregion

  UIAddToolBar( m_hWndCECommandBar );

  unsigned butWidth = pData->wWidth / nIcon;
  BOOL res = -1 != ::CommandBar_AddBitmap( m_hWndCECommandBar, hinst, nResourceID, nItems, butWidth, pData->wHeight );
  res = res && ::CommandBar_AddButtons( m_hWndCECommandBar, nItems, pButtons );

  m_notyfyIcon.Setup( m_hWnd, WM_ICON_NOTIFICATION, IDR_MAINFRAME );
  res = m_notyfyIcon.Install();

  CRect r;
  GetClientRect( &r );
  m_curClientWidth = r.Width();

  // register object for message filtering and idle updates
  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->AddMessageFilter(this);
  pLoop->AddIdleHandler(this);
  return 0;
}

LRESULT CMainFrame::OnKeyDown( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  bool bFirst = !(lParam & (1<<30));

  if ( !bFirst )
    return 0;

  switch ( wParam )
  {
    case VK_RETURN:
    {
      m_bFullScreen = !m_bFullScreen;
      UpdateScreenMode();
      bHandled = true;
    }
    break;

    case VK_DOWN:
    case VK_UP:
    case VK_LEFT:
    case VK_RIGHT:
      if ( m_pDjVuDoc )
      {
        StopTimer();
        m_scSate = SC_DN;
        RunTimerLong();
      }
      m_cursorKey = wParam;
      bHandled = true;
    break;
  }
  return 0;
}

LRESULT CMainFrame::OnKeyUp( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
  if ( !m_pDjVuDoc )
    return 0;

  switch ( wParam )
  {
    case VK_DOWN:
    case VK_UP:
    case VK_LEFT:
    case VK_RIGHT:
      if ( SC_L_TIMER == m_scSate )
      {
        switch ( wParam )
        {
          case VK_DOWN:
            OnPageUpDn( true );
          break;
          case VK_UP:
            OnPageUpDn( false );
          break;

          case VK_LEFT:
            OnPageLeftRight( true, true );
          break;

          case VK_RIGHT:
            OnPageLeftRight( false, true );
          break;
        }
      }
      StopTimer();
      m_scSate = SC_START;
      m_cursorKey = 0;
      bHandled = true;
    break;    
  }  
    
  return 0;
}

void CMainFrame::RunTimerLong()
{
  ATLASSERT( SC_DN == m_scSate );
  m_timerID = SetTimer( 1, g_cLongScrollTimeout );
  m_scSate = SC_L_TIMER;
}

void CMainFrame::RunTimerShort()
{
  ATLASSERT( SC_SH_TIMER == m_scSate );
  m_timerID = SetTimer( 1, g_cShortScrollTimeout );
}

void CMainFrame::StopTimer()
{
  if ( !m_timerID )
    return;

  KillTimer( m_timerID );
  m_timerID = 0;
}

LRESULT CMainFrame::OnTimer( UINT /*uMsg*/, WPARAM wParamIsTimerID, LPARAM /*lParam*/, BOOL& bHandled )
{
  if ( 1 != wParamIsTimerID )
    return 0;
  bHandled = true;

  ASSERT( SC_L_TIMER == m_scSate || SC_SH_TIMER == m_scSate );
  m_scSate = SC_SH_TIMER;
  RunTimerShort();

  switch ( m_cursorKey )
  {
    case VK_DOWN:
      OnPageUpDn( true, false );
      break;
    case VK_UP:
      OnPageUpDn( false, false );
      break;

    case VK_LEFT:
      OnPageLeftRight( true, false );
      break;

    case VK_RIGHT:
      OnPageLeftRight( false, false );
    break;
  }

  return 0;
}

bool CMainFrame::IsVisible( CRect const & rect )
{
  CRect rc;
  GetClientRect( &rc );

  if ( (rc.top   <= rect.top    && rect.top    <= rc.bottom)   ||
       (rc.top   <= rect.bottom && rect.bottom <= rc.bottom)   ||
       (rect.top <= rc.top      && rc.top      <= rect.bottom) )
  {
    return true;
  }

  return false;
}
void CMainFrame::OnPageUpDn( bool bDown, bool bByPage )
{
  if ( !m_pDjVuDoc )
    return;

  CRect rc;
  GetClientRect( &rc );
  int dy = 0;
  if ( bByPage )
  {
    dy = int(rc.Height() * g_cPageScrollK);
  }
  else
  {
    dy = g_cPixelsInLineScroll;
  }
  if ( bDown )
    dy = -dy;
  
  PageLayout( dy );

  Invalidate();
}

void CMainFrame::OnPageLeftRight( bool toRight, bool bByPage )
{
  if ( !m_pDjVuDoc )
    return;

  CRect clientRect;
  if ( !GetClientRect( &clientRect ) )
    return;
  
  int dX = g_cPixelsInLineScroll;
  if ( bByPage )
  {
    dX = clientRect.Width();
    dX = dX * 8 / 10; // TODO: move this constant to settings
  }
  dX *= toRight ? 1 : -1;

  ScrollPagesHor( dX );
  Invalidate();
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  PostMessage(WM_CLOSE);
  return 0;
}

void CMainFrame::AppSave()
{
  if ( m_bDirty && m_pDjVuDoc )
  {
    m_bDirty = false;
    m_mru[ 0 ].m_pageNum = Get1stVisiblePage();

    int j=-1;
    for ( int i=0; i<g_cMruNumber; ++i )
    {
      if ( m_mru[ i ].m_curFillFileName.IsEmpty() )
        continue;
      ++j;
      CString file;
      file.Format( L"MRU_file_%d", j );
      CString page;
      page.Format( L"MRU_page_%d", j );

      m_appInfo.Save( m_mru[ i ].m_curFillFileName, (LPCWSTR)file );
      m_appInfo.m_Key.SetDWORDValue( page, m_mru[ i ].m_pageNum );
    }
  }
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  bHandled = true;
  
  CFullScrnOnOff fullScrOnOff( *this );

#if UNDER_CE < 0x500
  CFileDialog fd( TRUE, L"djvu", 0, 0, L"Djvu files (*.djvu)\0*.djvu\0" );
  if ( IDOK != fd.DoModal() )
  {
    return 0;
  }

  int pageIndex = GetPageIndFromMru( fd.m_szFileName );
  OpenFile( fd.m_szFileName, pageIndex );
#else
  wchar_t szFile[MAX_PATH] = {0};
  OPENFILENAMEEX  ofnex = {0};

  ofnex.lStructSize     = sizeof ofnex;
  ofnex.hwndOwner       = m_hWnd;
  ofnex.lpstrFile       = szFile;
  ofnex.nMaxFile        = sizeof(szFile)/sizeof(szFile[0]);
  ofnex.lpstrFilter     = L"DjVu files (*.djvu)\0*.djvu\0";
  ofnex.lpstrTitle      = L"Open file";

  // Show thumbnails of files that are not DRM protected
  ofnex.ExFlags         = OFN_EXFLAG_DETAILSVIEW | OFN_EXFLAG_NOFILECREATE; //??? OFN_EXFLAG_EXLORERVIEW
  ofnex.lpstrInitialDir = m_initDir;

  if ( !GetOpenFileNameEx( &ofnex ) )
  {
    return 0;
  }

  int pageIndex = GetPageIndFromMru( ofnex.lpstrFile );
  OpenFile( ofnex.lpstrFile, pageIndex );
#endif // UNDER_CE

  return 0;
}

bool CMainFrame::AppNewInstance( LPCTSTR lpstrCmdLine )
{
  int pageIndex = GetPageIndFromMru( lpstrCmdLine );
  return OpenFile( lpstrCmdLine, pageIndex );
}

bool CMainFrame::OpenFile( LPCWSTR fullFileName, int pageIndex )
{
  CWaitCursor wc;
  // TODO: check why DjVuLibre doesn't like Cyrillic in the file names.
  CString fileName( L"file://localhost" );
  fileName += fullFileName;
  fileName.Replace( '\\', '/' );

  int strL = WideCharToMultiByte( CP_ACP, 0, fileName, fileName.GetLength(), 0, 0, 0, 0 );
  char * ansiUrl = (char*)_alloca( strL+1 );
  memset( ansiUrl, 0, strL+1 );
  WideCharToMultiByte( CP_ACP, 0, fileName, fileName.GetLength(), ansiUrl, strL, 0, 0 );

  GNativeString nativeUrl( ansiUrl );
  GURL url( nativeUrl );
  GP<DjVuDocument> pDjVuDoc = DjVuDocument::create( url );
  if ( !pDjVuDoc || !pDjVuDoc->wait_for_complete_init() )
    return false;
  
  int pagesInDoc = pDjVuDoc->get_pages_num();
  if( pagesInDoc <= 0 )
    return false;
  if ( pagesInDoc <= pageIndex )
  {
    pageIndex = 0;
  }  

  CRect clientRect;
  GetClientRect( &clientRect );
  PagePtr pPage( new CPage( m_hWnd, pDjVuDoc, clientRect, true, pageIndex ) );
  if ( !pPage->LoadBmpSync() )
    return false;

#pragma region Commit    
  m_Pages.clear();
  m_Pages.push_back( pPage );
  m_pDjVuDoc = pDjVuDoc;
  SetCurFileInMru( fullFileName, pageIndex );
  UpdateMruMenu();
  m_bDirty = true;
#pragma endregion

  {
    wchar_t fDir[_MAX_PATH];
    wchar_t fName[_MAX_PATH];
    wchar_t fExt[_MAX_PATH];
    _wsplitpath_s( fullFileName,
                   0,0,
                   fDir,sizeof(fDir)/sizeof(fDir[0]),
                   fName,sizeof(fName)/sizeof(fName[0]),
                   fExt,sizeof(fExt)/sizeof(fExt[0]) );
    m_initDir = fDir;
    CString head( fName );
    head += fExt;
    SetWindowText( head );
  }
  PageLayout();

  return true;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CFullScrnOnOff fs( *this );
  CAboutDlg dlg;
  dlg.DoModal();
  return 0;
}

LRESULT CMainFrame::OnToolsOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: implement options dialog
  MessageBox( L"OnToolsOptions", L"TODO..." );

  return 0;
}

void CMainFrame::UpdateScreenMode()
{
  if ( !m_bFullScreen )
  {
    BOOL bRes = ::SHFullScreen( m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON );

    if ( m_hWndCECommandBar )
      ::ShowWindow( m_hWndCECommandBar, SW_SHOW );    

    CRect rtDesktop( 0, 52, 200, 148 );
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rtDesktop, NULL );

    CRect cmdBarRect;
    ::GetWindowRect( m_hWndCECommandBar, &cmdBarRect );

    rtDesktop.bottom = cmdBarRect.top;

    SetWindowPos( HWND_TOP,
                  rtDesktop.left,
                  rtDesktop.top,
                  rtDesktop.Width(),
                  rtDesktop.Height(), 
                  0
                );
  }
  else
  {
    BOOL bRes = ::SHFullScreen( m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );

    if ( m_hWndCECommandBar )
      ::ShowWindow( m_hWndCECommandBar, SW_HIDE );

    SetWindowPos( HWND_TOP,
                  0,0,
                  GetSystemMetrics(SM_CXSCREEN),
                  GetSystemMetrics(SM_CYSCREEN), 
                  0
                );
  }
}

void CMainFrame::DoPaint( CDCHandle dc )
{
  using namespace WTL;
  CRect rect;
  GetClientRect( &rect );

  dc.FillRect( rect, (HBRUSH)GetStockObject(DKGRAY_BRUSH) );
  
  if ( !m_pDjVuDoc )
    return;

  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    (*i)->Draw( dc );
  }
}

LRESULT CMainFrame::OnActivate( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  WORD fActive = LOWORD(wParam); 
  
  if ( WA_INACTIVE != fActive )
  { // Activate!
    UpdateScreenMode();
  }
  bHandled = true;
  return 0;
}

LRESULT CMainFrame::OnSettingChange( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{
  // TODO: check if window is not active and postpone image reloading
  bHandled = true;
  if ( !m_pDjVuDoc || m_Pages.empty() || SETTINGCHANGE_RESET != wParam )
    return 0;

  CRect cr;
  GetClientRect( &cr );
  int width = cr.Width();

  if ( m_curClientWidth != width )
  {
    PagePtr pg = GetCurrentPage();
    m_Pages.clear();
    CRect r = pg->GetRect();
    double k = double(width) / m_curClientWidth;
    r.left   = Round( k * r.left );
    r.top    = Round( k * r.top );
    r.right  = Round( k * r.right );
    r.bottom = Round( k * r.bottom );

    m_curClientWidth = width;
   
    Invalidate();
    pg.Reset( new CPage( m_hWnd, m_pDjVuDoc, r, true, pg->GetPageIndex() ) );
    CWaitCursor wc;
    if ( !pg->LoadBmpSync() )
    {
      // TODO: restore previous state, because we have here all pages are deleted
      return 0;
    }
    m_Pages.push_back( pg );
    m_bDirty = true;
    PageLayout();
  }

  return 0;
}

LRESULT CMainFrame::OnZoomCmd( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{
  bHandled = true;
  if ( !m_pDjVuDoc )
    return 0;

  CRect r;
  if ( !GetClientRect( r ) )
    return 0;

  int const zoomD = (int)min( r.Width()*g_cZoomK, r.Height()*g_cZoomK );

  switch ( wID )
  {
    case ID_ZOOM_ZOOMIN:
      r.DeflateRect( zoomD, zoomD );
    break;

    case ID_ZOOM_ZOOMOUT:
      r.InflateRect( zoomD, zoomD );
    break;

    case ID_ZOOM_FITSCREENWIDTH:
      {
        PagePtr pg = GetCurrentPage();
        if ( !pg )
          return 0;
        r = pg->GetRect();
        CPoint c = r.CenterPoint();
        r.top    = c.y - g_cMinZoomRect/2;
        r.bottom = c.y + g_cMinZoomRect/2;
      }
    break;

    case ID_ZOOM_FITSCREENHEIGHT:
      {
        PagePtr pg = GetCurrentPage();
        if ( !pg )
          return 0;
        r = pg->GetRect();
        CPoint c = r.CenterPoint();
        r.left  = c.x - g_cMinZoomRect/2;
        r.right = c.x + g_cMinZoomRect/2;
      }
    break;

    case ID_ZOOM_ZOOMBYRECT:
      if ( m_pZoomCtr )
      {
        FinishZoomCtrl();
      }
      else
      {
        m_pZoomCtr.Reset( new CRectZoomCtrl( this ) );
        SetChainEntry( 0, m_pZoomCtr.GetPtr() );
        UISetCheck(ID_ZOOM_ZOOMBYRECT, true);
      }
      return 0;
    break;

    case ID_ZOOM_FITPAGE:
      {
        PagePtr pg = GetCurrentPage();
        if ( !pg )
          return 0;
        r = pg->GetRect();
      }
    break;

    default:
    return 0;
  }

  CalcZoomKandOffset( r );
  return 0;
}

void CMainFrame::ScrollPagesVert( int & moveY )
{
  if ( !moveY )
    return;

  if ( moveY>0 )
  {
    PagePtr & pg = *m_Pages.begin();
    if ( !pg->GetPageIndex() )
    {
      CRect r = pg->GetRect();
      r.top += moveY;
      if (  r.top>0 )
      {
        moveY -= r.top;
      }
    }
  }
  else if ( moveY<0 )
  {
    int const lastInex = m_pDjVuDoc->get_pages_num()-1;    
    PagePtr & pg = *m_Pages.rbegin();
    if ( lastInex == pg->GetPageIndex() )
    {
      CRect cr;
      GetClientRect( &cr );
      CRect r = pg->GetRect();
      r.bottom += moveY;
      if ( r.bottom<cr.bottom )
      {
        moveY += cr.bottom-r.bottom;
      }
    }
  }
 
  if ( !moveY )
    return;

  m_bDirty = true;

  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    (*i)->Scroll( 0, moveY );    
  }
}

void CMainFrame::ScrollPagesHor( int & moveX )
{
  if ( !moveX )
    return;

  CRect r = (*m_Pages.begin())->GetRect();
  if ( moveX < 0 )
  {
    r.right += moveX;
    if ( r.right <= 0 )
      return;
  }
  else
  {
    CRect cr;
    GetClientRect( &cr );
    r.left += moveX;
    if ( r.left >= cr.right )
      return;
  }

  m_bDirty = true;

  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    PagePtr & p = *i;
    p->Scroll( moveX, 0 );
  }
}

void CMainFrame::ClearRedundantCache()
{
  int const cacheMax = 2;
  int i1 = -1;
  int i2 = -1;
  
  int c = 0;  
  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i, ++c )
  {
    if ( IsVisible( (*i)->GetRect() ) )
    {
      i1 = c;
      break;
    }
  }
  
  c = m_Pages.size()-1;
  for ( Pages::reverse_iterator i=m_Pages.rbegin(); i!=m_Pages.rend(); ++i, --c )
  {
    if ( IsVisible( (*i)->GetRect() ) )
    {
      i2 = c;
      break;
    }
  }

  for ( c = m_Pages.size()-1; c-i2 > cacheMax; --c )
  {
    m_Pages.pop_back();
  }

  for ( c = 0; i1-c > cacheMax; ++c )
  {
    m_Pages.pop_front();
  }
}

void CMainFrame::AddVisibleButNotLoaded()
{
  if ( m_Pages.empty() )
    return;

  CRect r;
  CWaitCursor wc;
  while ( true )
  {
    int index = m_Pages.front()->GetPageIndex() - 1;
    if ( index < 0 )
      break;
    
    r = m_Pages.front()->GetRect();
    r.MoveToY( r.top - r.Height() - g_cPageGap );
    if ( !IsVisible( r ) )
      break;

    PagePtr p( new CPage( m_hWnd, m_pDjVuDoc, r, true, index ) );
    if ( !p->LoadBmpSync() )
    {
      break;
    }
    m_Pages.push_front( p );
  }

  // ...
  while ( true )
  {
    int index = m_Pages.back()->GetPageIndex() + 1;
    if ( index >= m_pDjVuDoc->get_pages_num() )
      break;
    
    r = m_Pages.back()->GetRect();
    r.MoveToY( r.top + r.Height() + g_cPageGap );
    if ( !IsVisible( r ) )
      break;

    PagePtr p( new CPage( m_hWnd, m_pDjVuDoc, r, true, index ) );
    if ( !p->LoadBmpSync() )
    {
      break;
    }
    m_Pages.push_back( p );
  }
}

void CacheNext()
{
  // Todo:
}

void CachePrev()
{
  // Todo:
}

void CMainFrame::PageLayout( int moveY /*= 0*/ )
{
  int curInd = 0;
  PagePtr pCurPg = GetCurrentPage( &curInd );

  ScrollPagesVert( moveY );
  
  ClearRedundantCache();
  AddVisibleButNotLoaded();

  if ( !moveY )
    return;

  if ( moveY <= 0 )
    CacheNext();
  else
    CachePrev();
}

int CMainFrame::Get1stVisiblePage()
{
  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    if ( IsVisible( (*i)->GetRect() ) )
    {
      return (*i)->GetPageIndex();
    }
  }
  return 0;
}

PagePtr CMainFrame::GetCurrentPage( int * pIndex /*= 0*/ )
{
  CRect clientRec;
  GetClientRect( &clientRec );
  CPoint clientCenter = clientRec.CenterPoint();
  
  PagePtr p1stVis;
  PagePtr p1st;

  int index1stVis = 0;
  int index1st = 0;
  int j=0;
  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i, ++j )
  {
    CRect pgRect = (*i)->GetRect();
    if ( pgRect.top     <= clientCenter.y && 
         clientCenter.y <= pgRect.bottom )
    {
      if ( pIndex )
        *pIndex = j;
      return *i;
    }

    if ( !p1stVis && IsVisible( pgRect ) )
    {
      index1stVis = j;
      p1stVis = *i;
    }
    
    if ( !p1st )
    {
      index1st = j;
      p1st = *i;
    }
  }

  if ( p1stVis )
  {
    if ( pIndex )
      *pIndex = index1stVis;
    return p1stVis;
  }

  if ( pIndex )
      *pIndex = index1st;
  return p1st;
}

void CMainFrame::LoadSettings()
{  
  int j=0;
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    m_mru[ i ].m_curFillFileName = L"";
    m_mru[ i ].m_pageNum = 0;

    CString file;
    file.Format( L"MRU_file_%d", i );
    CString page;
    page.Format( L"MRU_page_%d", i );

    CString path;
    DWORD pg;
    if ( !m_appInfo.Restore( path, (LPCWSTR)file ) &&
         !m_appInfo.m_Key.QueryDWORDValue( page, pg ) &&
         !path.IsEmpty() )
    {
      HANDLE h = ::CreateFile( path,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               0 );
      if ( INVALID_HANDLE_VALUE == h )
      {
        continue;
      }
      ::CloseHandle( h );

      m_mru[ j ].m_curFillFileName = path;
      m_mru[ j ].m_pageNum = pg;
      ++j;
    }
  }

  bool bLoaded = false;
  if ( !m_cmdLine.IsEmpty() )
  {
    bLoaded = AppNewInstance( m_cmdLine );
  }

  if ( !bLoaded && !m_mru[ 0 ].m_curFillFileName.IsEmpty() )
  {
    OpenFile( m_mru[ 0 ].m_curFillFileName, m_mru[ 0 ].m_pageNum );
  }
}

void CMainFrame::SetCurFileInMru( CString const & fullFileName, int pageIndex )
{
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( fullFileName == m_mru[i].m_curFillFileName )
    {
      CMru t = m_mru[i];
      for ( int j=i; j>0; --j )
      {
        m_mru[j] = m_mru[j-1];
      }
      m_mru[ 0 ] = t;
      return;
    }
  }

  for ( int i=g_cMruNumber-1; i>0; --i )
  {
    m_mru[i] = m_mru[i-1];
  }
  m_mru[ 0 ].m_curFillFileName = fullFileName;
  m_mru[ 0 ].m_pageNum = pageIndex;
}

int CMainFrame::GetPageIndFromMru( CString const & fileFullPath )
{
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( fileFullPath == m_mru[i].m_curFillFileName )
    {
      return m_mru[i].m_pageNum;
    }
  }
  return 0;
}

void CMainFrame::UpdateMruMenu()
{
  HMENU hMenu = (HMENU)::SendMessage( m_hWndCECommandBar, SHCMBM_GETMENU, 0, 0 );
  if ( !hMenu )
    return;

  CMenuHandle mnu;
  mnu.Attach( hMenu );
  CMenuHandle sub0Mnu = mnu.GetSubMenu( 0 );
  ATLASSERT( sub0Mnu );
  if ( !sub0Mnu )
    return;

  CMenuHandle mruMnu = sub0Mnu.GetSubMenu( 6 );
  ATLASSERT( mruMnu );
  if ( !mruMnu )
    return;

#pragma region Cleanup
  int mCnt = ::GetMenuItemCount( mruMnu );
  while( mCnt-- )
  {
    mruMnu.RemoveMenu( mCnt, MF_BYPOSITION );
  }
#pragma endregion

  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( m_mru[i].m_curFillFileName.IsEmpty() )
      continue;
 
    wchar_t fName[_MAX_PATH];
    _wsplitpath_s( (LPCWSTR)m_mru[i].m_curFillFileName,
                   0,0,
                   0,0,
                   fName,sizeof(fName)/sizeof(fName[0]),
                   0,0 );
    mruMnu.AppendMenuW( 0, ID_FILE_MRU_FILE1+i, fName );
  }
}

LRESULT CMainFrame::OnMru( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & bHandled )
{
  unsigned i = wID - ID_FILE_MRU_FILE1;
  if ( g_cMruNumber <= i || m_mru[i].m_curFillFileName.IsEmpty() )
    return 1;

  if ( OpenFile( m_mru[i].m_curFillFileName, m_mru[i].m_pageNum ) )
  {
    Invalidate();
  }
  else
  {    
    for ( unsigned j=i+1; j<g_cMruNumber; ++j )
    {
      m_mru[j-1] = m_mru[j];
    }
    m_mru[g_cMruNumber-1].Clear();
    UpdateMruMenu();
  }
  bHandled = true;
  return 0;
}
LRESULT CMainFrame::OnNavigationGotopage( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  bHandled = true;
  if ( !m_pDjVuDoc )
    return 0;

  CFullScrnOnOff fullScrOnOff( *this );

  int visPg = 1+Get1stVisiblePage();
  CGoToPage gotoPg( m_pDjVuDoc->get_pages_num(), visPg );
  if ( IDCANCEL == gotoPg.DoModal( m_hWnd ) )
  {
    return 0;
  }

  int pg = gotoPg.GetPage();
  if ( visPg == pg )
  {
    return 0;
  }
  --pg;

  CRect clientRect;
  if ( !GetClientRect( &clientRect ) )
  {
    return 0;
  }
  
  Invalidate();
  CWaitCursor wc;
  
  m_Pages.clear();
  PagePtr pPage( new CPage( m_hWnd, m_pDjVuDoc, clientRect, true, pg ) );
  if ( !pPage->LoadBmpSync() )
    return 0;
  
  m_Pages.push_back( pPage );
  m_mru[0].m_pageNum = pg;
  m_bDirty = true;
  PageLayout();

  return 0;
}

void CMainFrame::FinishZoomCtrl()
{
  if ( m_pZoomCtr )
  {
    UISetCheck(ID_ZOOM_ZOOMBYRECT, false);
    m_pZoomCtr.Reset();
    RemoveChainEntry( 0 );
  }
}

void CMainFrame::FinishCtrl( void * pSourceCtrl, bool bCancel )
{
  if ( pSourceCtrl == m_pZoomCtr )
  {
    if ( !bCancel )
    {
      CRect r = m_pZoomCtr->GetRect();
      CalcZoomKandOffset( r );
    }
    FinishZoomCtrl();
  }
}

void CMainFrame::CalcZoomKandOffset( CRect & r )
{
  r.NormalizeRect();

  if ( r.Width() < g_cMinZoomRect && r.Height() < g_cMinZoomRect )
    return;
  
  CRect cr;
  if ( !GetClientRect( &cr ) )
    return;

  double rx = ((double)cr.Width())/r.Width();
  double ry = ((double)cr.Height())/r.Height();
  double zoomK  = (rx < ry ? rx : ry);

  if ( g_cZoomMaxK < zoomK )
    zoomK = g_cZoomMaxK;
  if ( zoomK < g_cZoomMinK )
    zoomK = g_cZoomMinK;
    
  CPoint clientCenter = cr.CenterPoint();
  CPoint newWinCenter = r.CenterPoint();

  PagePtr pCurPg = GetCurrentPage();
  m_Pages.clear();

  int index = pCurPg->GetPageIndex();
  CRect pR = pCurPg->GetRect();
  CPoint newImgCenter = newWinCenter - pR.TopLeft();

  pR.right  = Round(zoomK * pR.Width() );
  pR.bottom = Round(zoomK * pR.Height());
  pR.left   = 0;
  pR.top    = 0;

  newImgCenter.x   = Round(zoomK * newImgCenter.x);
  newImgCenter.y   = Round(zoomK * newImgCenter.y);

  CPoint v = clientCenter - newImgCenter;

  pR.left   += v.x;
  pR.right  += v.x;
  pR.top    += v.y;
  pR.bottom += v.y;
 
  CWaitCursor wc;
  Invalidate();
  pCurPg.Reset( new CPage( m_hWnd, m_pDjVuDoc, pR, true, index ) );
  if ( !pCurPg->LoadBmpSync() )
  {
    // TODO: restore previous mode, because here we have all pages are deleted
    return;
  }
 
  m_Pages.push_back( pCurPg );
  m_bDirty = true;
  PageLayout();
}

LRESULT CMainFrame::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;

  SHRGINFO shrg = {0};
  shrg.cbSize     = sizeof shrg;
  shrg.hwndClient = m_hWnd;
  shrg.ptDown.x   = GET_X_LPARAM(lParam);
  shrg.ptDown.y   = GET_Y_LPARAM(lParam);
  shrg.dwFlags    = SHRG_RETURNCMD;
  
  if ( GN_CONTEXTMENU != SHRecognizeGesture( &shrg ) )
    return 0;

  HMENU hMenu = (HMENU)::SendMessage( m_hWndCECommandBar, SHCMBM_GETMENU, 0, 0 );
  if ( !hMenu )
    return 0;

  CMenuHandle mnu;
  mnu.Attach( hMenu );
  CMenuHandle sub0Mnu = mnu.GetSubMenu( 0 );
  ATLASSERT( sub0Mnu );
  if ( !sub0Mnu )
    return 0;

  sub0Mnu.TrackPopupMenu( 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), m_hWnd );

  return 0;
}

LRESULT CMainFrame::OnTrayNotyfy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  bHandled = true;

  switch( lParam )
  {
    case WM_LBUTTONUP:
      ShowWindow(SW_RESTORE);
      SetForegroundWindow(m_hWnd);
    break;
  }

  return 0;
}
LRESULT CMainFrame::OnFullscreenCmd( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  bHandled = true;

  m_bFullScreen = !m_bFullScreen;
  UpdateScreenMode();

  HMENU hMenu = (HMENU)::SendMessage( m_hWndCECommandBar, SHCMBM_GETMENU, 0, 0 );
  if ( !hMenu )
    return 0;

  CMenuHandle mnu;
  mnu.Attach( hMenu );
  CMenuHandle sub0Mnu = mnu.GetSubMenu( 0 );
  ATLASSERT( sub0Mnu );
  if ( !sub0Mnu )
    return 0;

  sub0Mnu.CheckMenuItem( ID_FULLSCREEN, MF_BYCOMMAND | (m_bFullScreen ? MF_CHECKED : MF_UNCHECKED) );

  return 0;
}
