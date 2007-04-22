// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../libdjvu/DjVuImage.h"
#include "../libdjvu/GBitmap.h"

#include "resource.h"

#include "./aboutdlg.h"
#include "./MainFrm.h"
#include "./PageLoader.h"
#include "./GotoPage.h"
#include "./ScrollByTap.h"
#include "./MoveByStylus.h"
#include "./BookmarkDlg.h"
#include "./OptionsDlg.h"
#include "./misc.h"

CMainFrame::CMainFrame() :
  m_bFullScreen(false)
  , m_curClientWidth()
  , m_1stClick()
  , m_initDir(L"\\")
  , m_appInfo( APP_REG_PATH )
  , m_scSate( SC_START )
  , m_timerID()
  , m_cursorKey()
  , m_cmdIDCntrl()
  , m_bDirty()
  , m_histStartInd()
  , m_histCurInd()
  , m_maxHistL(g_cHistoryLength)
{
  m_toolTips.reserve(16);
  m_toolTips.push_back( L"" ); // For 1st menu button (i.e. "*").

  m_history.reserve( m_maxHistL );  
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
  DWORD disen = m_Pages.empty() ? UPDUI_DISABLED : UPDUI_ENABLED;
  DWORD disenMask = ~(UPDUI_DISABLED | UPDUI_ENABLED);

  DWORD state = 0;
  state = disen | ( UIGetState( ID_ZOOM_ZOOMBYRECT ) & disenMask );
  UISetState( ID_ZOOM_ZOOMBYRECT, state );

  state = disen | ( UIGetState( ID_SCROLL_BY_TAP ) & disenMask );
  UISetState( ID_SCROLL_BY_TAP,   state );
  
  state = disen | ( UIGetState( ID_MOVE_BY_STYLUS ) & disenMask );
  UISetState( ID_MOVE_BY_STYLUS,  state );
  //................
  UISetState( ID_NAVIGATE_ADDBOOKMARK,  disen );
  UISetState( ID_NAVIGATION_GOTOPAGE,   disen );
  UISetState( ID_NAVIGATION_BACK,       disen );
  UISetState( ID_NAVIGATION_FORWARD,    disen );
  UISetState( ID_NAVIGATION_HISTORY,    disen );

  UISetState( ID_ZOOM_ZOOMIN,           disen );
  UISetState( ID_ZOOM_ZOOMOUT,          disen );
  UISetState( ID_ZOOM_FITSCREENWIDTH,   disen );
  UISetState( ID_ZOOM_FITSCREENHEIGHT,  disen );
  UISetState( ID_ZOOM_FITPAGE,          disen );
  //................
  state = UPDUI_ENABLED;
  state |= m_bFullScreen ? UPDUI_CHECKED : 0 ;
  UISetState( ID_FULLSCREEN, state );
  //................

  UIUpdateToolBar();
  AppSave();
  return FALSE;
}

void CMainFrame::LoadTooltipStr( DWORD id )
{
  wchar_t const * pTooltipStr = (wchar_t const *)LoadString( _Module.GetModuleInstance(), id, NULL, 0 );  
  if ( !pTooltipStr )
  {
    m_toolTips.push_back( L"" );
  }
  else
  {
    // Make sure that the RC.EXE is invoked with the "-n" option.
    WORD numChars = *(-1+(WORD*)pTooltipStr);
    WORD numCharsCnt = wcslen(pTooltipStr) + 1; //+1 - zero terminal
    ATLASSERT( numChars == numCharsCnt );

    m_toolTips.push_back( pTooltipStr );
  }
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  HINSTANCE hinst = _Module.GetModuleInstance();

  SHMENUBARINFO mbi = { 0 };
  mbi.cbSize      = sizeof(mbi);
  mbi.hwndParent  = m_hWnd;
  mbi.dwFlags     = SHCMBF_HMENU | (g_cGUItoolBarUseWinCapBkgrnd ? SHCMBF_COLORBK : 0);
  mbi.nToolBarId  = IDR_MAINFRAME;
  mbi.hInstRes    = hinst;
  mbi.clrBk       = GetSysColor( COLOR_ACTIVECAPTION );

  BOOL bRet = ::SHCreateMenuBar( &mbi );
  ATLASSERT( bRet );
  
  m_hWndCECommandBar = mbi.hwndMB;  
  
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
      pButtons[i].iBitmap = nIcon;
      pButtons[i].idCommand = pItems[i];
      pButtons[i].fsState = TBSTATE_ENABLED;
      pButtons[i].fsStyle = TBSTYLE_BUTTON;

      LoadTooltipStr( pItems[i] );

      ++nIcon;
    }
    else
    {
      pButtons[i].fsStyle = TBSTYLE_SEP;
    }
  }  
#pragma endregion

  UIAddToolBar( m_hWndCECommandBar );
  
  nResourceID = IsVGA() ? IDB_TB_VGA : IDB_TB_QVGA;
  
  // The following call creates actually the toolbar as child window to MenuBar.
  BOOL res = -1 != ::CommandBar_AddBitmap( m_hWndCECommandBar, hinst, nResourceID, nIcon, 0, 0 );
  res = res && ::CommandBar_AddButtons( m_hWndCECommandBar, nItems, pButtons );
  res = res && ::CommandBar_AddToolTips( m_hWndCECommandBar, m_toolTips.size(), &m_toolTips[0] );
  
  // After the toolbar was created we can subclass it for our information painting.  
  m_tb.SubclassToolbar( m_hWndCECommandBar );
  m_tb.SetFrameWnd( m_hWnd );
  //...........................................................................
  m_notyfyIcon.Setup( m_hWnd, WM_ICON_NOTIFICATION, IDR_MAINFRAME );
  res = m_notyfyIcon.Install();  

  WTL::CRect r;
  GetClientRect( &r );
  m_curClientWidth = r.Width();

  // register object for message filtering and idle updates
  WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
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

  ASSERT( SC_L_TIMER == m_scSate || SC_SH_TIMER == m_scSate );
  StopTimer();
  m_scSate = SC_SH_TIMER;

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

  RunTimerShort();

  return 0;
}

bool CMainFrame::IsVisible( WTL::CRect const & rect )
{
  WTL::CRect rc;
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

  WTL::CRect rc;
  GetClientRect( &rc );
  int dy = 0;
  if ( bByPage )
  {
    dy = rc.Height() * g_cPageScrollVertPercent / 100;
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

  WTL::CRect clientRect;
  if ( !GetClientRect( &clientRect ) )
    return;
  
  int dX = g_cPixelsInLineScroll;
  if ( bByPage )
  {
    dX = clientRect.Width();
    dX = dX * g_cPageScrollHorPercent / 100;
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

void CMainFrame::HistoryCleanup()
{
  m_histStartInd = m_histCurInd = 0;
  m_history.clear();
}

void CMainFrame::UpdateHistory( PagePtr p1sVisPg )
{
  if ( !p1sVisPg )
  {
    return;
  }

  CBookmarkInfo bi( p1sVisPg->GetPageIndex(), p1sVisPg->GetRect() );
  int inc = m_history.empty() || m_history[ m_histCurInd ].m_pageIndex == bi.m_pageIndex
            ? 0 : 1; // update bookmark only or insert new history item.
  
  m_histCurInd = (m_histCurInd + inc) % m_maxHistL;
  if ( int(m_history.size()) <= m_histCurInd )
  {        
    m_history.push_back( bi );
  }
  else
  {
    m_history[ m_histCurInd ] = bi;
  }
  if ( inc && m_histCurInd == m_histStartInd )
  {
    m_histStartInd = (m_histCurInd + 1) % m_maxHistL;
  }
}

void CMainFrame::AppSave()
{
  if ( !m_bDirty || !m_pDjVuDoc )
  {
    return;
  }
  
  m_bDirty = false;
  PagePtr p1sVis = Get1stVisiblePage();
  CBookmarkInfo bm;
  if ( p1sVis )
  {
    bm.m_pageIndex = p1sVis->GetPageIndex();
    bm.m_pageRect  = p1sVis->GetRect();
  }
  
  m_tb.SetPages( bm.m_pageIndex + 1, m_pDjVuDoc->get_pages_num() );

  UpdateHistory( p1sVis );

  int j=-1;
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( m_mru[ i ].IsEmpty() )
      continue;
    ++j;
    WTL::CString file;
    file.Format( L"MRU_file_%d", j );
      
    m_appInfo.Save( m_mru[ i ], (LPCWSTR)file );
  }
  
  CBookmarkDlg::SaveAutoBM( m_mru[ 0 ], bm );
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  CFullScrnOnOff fullScrOnOff( *this );

#if UNDER_CE < 0x500
  CFileDialog fd( TRUE, L"djvu", 0, 0, L"Djvu files (*.djvu)\0*.djvu\0" );
  if ( IDOK != fd.DoModal() )
  {
    return 0;
  }

  CBookmarkInfo bm;
  CBookmarkDlg::LoadAutoBM( fd.m_szFileName, bm );
  OpenFile( fd.m_szFileName, bm );
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

  CBookmarkInfo bm;
  CBookmarkDlg::LoadAutoBM( ofnex.lpstrFile, bm );
  OpenFile( ofnex.lpstrFile, bm );
#endif // UNDER_CE

  return 0;
}

bool CMainFrame::AppNewInstance( LPCTSTR lpstrCmdLine )
{

  CBookmarkInfo bm;
  CBookmarkDlg::LoadAutoBM( lpstrCmdLine, bm );
  return OpenFile( lpstrCmdLine, bm );
}

bool CMainFrame::OpenFile( LPCWSTR fullFileName, CBookmarkInfo bm )
{
  m_tb.SetPages();
  WTL::CWaitCursor wc;
  WTL::CString fileName( L"file://localhost" );
  fileName += fullFileName;
  fileName.Replace( '\\', '/' );

  int strL = WideCharToMultiByte( CP_UTF8, 0, fileName, fileName.GetLength(), 0, 0, 0, 0 );
  char * utf8iUrl = (char*)_alloca( strL+1 );
  memset( utf8iUrl, 0, strL+1 );
  WideCharToMultiByte( CP_UTF8, 0, fileName, fileName.GetLength(), utf8iUrl, strL, 0, 0 );

  GNativeString nativeUrl( utf8iUrl );
  GURL url( nativeUrl );
  GP<DjVuDocument> pDjVuDoc = DjVuDocument::create( url );
  if ( !pDjVuDoc || !pDjVuDoc->wait_for_complete_init() )
    return false;
  
  int pagesInDoc = pDjVuDoc->get_pages_num();
  if( pagesInDoc <= 0 )
    return false;
  if ( pagesInDoc <= bm.m_pageIndex )
  {
    bm.m_pageIndex = 0;
  }  

  PagePtr pPage( new CPage( m_hWnd, pDjVuDoc, bm.m_pageRect, true, bm.m_pageIndex ) );
  if ( !pPage->LoadBmpSync() )
  {
    // TODO: show something reasonable (like ShowZoomWarning();)
    return false;
  }

#pragma region Commit    
  m_Pages.clear();
  m_Pages.push_back( pPage );
  m_pDjVuDoc = pDjVuDoc;
  HistoryCleanup();
  SetCurFileInMru( fullFileName );
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
    WTL::CString head( fName );
    head += fExt;
    SetWindowText( head );
  }
  PageLayout();

  return true;
}

LRESULT CMainFrame::OnAppAbout( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  CFullScrnOnOff fs( *this );
  CAboutDlg dlg;
  dlg.DoModal();
  return 0;
}

LRESULT CMainFrame::OnToolsOptions( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  CFullScrnOnOff fs( *this );
  COptionsDlg dlg;
  dlg.DoModal( m_hWnd );
  return 0;
}

void CMainFrame::UpdateScreenMode()
{
  if ( !m_bFullScreen )
  {
    BOOL bRes = ::SHFullScreen( m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON );

    if ( m_hWndCECommandBar )
      ::ShowWindow( m_hWndCECommandBar, SW_SHOW );    

    WTL::CRect rtDesktop( 0, 52, 200, 148 );
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rtDesktop, NULL );

    WTL::CRect cmdBarRect;
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

void CMainFrame::DoPaint( WTL::CDCHandle dc )
{
  WTL::CRect rect;
  GetClientRect( &rect );

  dc.FillRect( rect, (HBRUSH)GetStockObject(DKGRAY_BRUSH) );
  
  if ( !m_pDjVuDoc )
    return;

  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    WTL::CRect r = (*i)->GetRect();
    // this checking reduces accesses to bitmaps potentially stored in the swap file.
    if ( !IsVisible( r ) )
    {
      continue;
    }
    (*i)->Draw( dc );
    
    // TODO: check if the r.Width > ScreenWidth
    ::SetWindowOrgEx( dc, r.Width() + g_cBetweenPageGap, 0, NULL );
    (*i)->Draw( dc );

    ::SetWindowOrgEx( dc, -r.Width() - g_cBetweenPageGap, 0, NULL );
    (*i)->Draw( dc );

    ::SetWindowOrgEx( dc, 0, 0, NULL );
  }
}

LRESULT CMainFrame::OnActivate( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
  WORD fActive = LOWORD(wParam); 
  
  if ( WA_INACTIVE != fActive )
  { // Activate!
    UpdateScreenMode();
  }

  return 0;
}

LRESULT CMainFrame::OnSettingChange( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
{
  // TODO: check if window is not active and postpone image reloading
  if ( !m_pDjVuDoc || m_Pages.empty() || SETTINGCHANGE_RESET != wParam )
    return 0;

  WTL::CRect cr;
  GetClientRect( &cr );
  int width = cr.Width();

  if ( m_curClientWidth != width )
  {
    PagePtr pCurPg = GetCurrentPage();
    m_Pages.clear();
    WTL::CRect r = pCurPg->GetRect();
    double k = double(width) / m_curClientWidth;
    r.left   = Round( k * r.left );
    r.top    = Round( k * r.top );
    r.right  = Round( k * r.right );
    r.bottom = Round( k * r.bottom );

    m_curClientWidth = width;
   
    Invalidate();
    PagePtr pNewPage( new CPage( m_hWnd, m_pDjVuDoc, r, true, pCurPg->GetPageIndex() ) );
    WTL::CWaitCursor wc;
    if ( !pNewPage->LoadBmpSync() )
    {
      m_Pages.push_back( pCurPg );
      ShowZoomWarning();
      return 0;
    }
    m_Pages.push_back( pNewPage );
    m_bDirty = true;
    PageLayout();
  }

  return 0;
}

void CMainFrame::ShowZoomWarning()
{
  WTL::CString szWarning;
  szWarning.LoadString( IDS_WARNING );

  WTL::CString szZoomTooMuch;
  szZoomTooMuch.LoadString( IDS_ZOOM_TOO_MUCH );

  ShowNotification( m_hWnd, szWarning, szZoomTooMuch );
}

LRESULT CMainFrame::OnZoomCmd( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled )
{

  if ( !m_pDjVuDoc )
    return 0;

  WTL::CRect r;
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
        WTL::CPoint c = r.CenterPoint();
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
        WTL::CPoint c = r.CenterPoint();
        r.left  = c.x - g_cMinZoomRect/2;
        r.right = c.x + g_cMinZoomRect/2;
      }
    break;

    case ID_ZOOM_ZOOMBYRECT:
      OnCtrlButton< ID_ZOOM_ZOOMBYRECT, CRectZoomCtrl >();
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
  if ( !moveY || m_Pages.empty() )
    return;

  if ( moveY>0 )
  {
    PagePtr & pg = *m_Pages.begin();
    if ( !pg->GetPageIndex() )
    {
      WTL::CRect r = pg->GetRect();
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
      WTL::CRect cr;
      GetClientRect( &cr );
      WTL::CRect r = pg->GetRect();
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
  if ( !moveX || m_Pages.empty() )
    return;

  WTL::CRect cr;
  GetClientRect( &cr );

  WTL::CRect const & r = (*m_Pages.begin())->GetRect();

  if ( 0 )
  {
    if ( moveX < 0 )
    {
      int x = r.right + moveX;
      if ( x <= 0 )
        return;
    }
    else
    {    
      int x = r.left + moveX;
      if ( x >= cr.right )
        return;
    }
  }
  else
  {
    int x = r.left + moveX;
    x = x % r.Width();
    moveX = x - r.left;
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

  for ( c = m_Pages.size()-1; c-i2 > g_cCacheMax; --c )
  {
    m_Pages.pop_back();
  }

  for ( c = 0; i1-c > g_cCacheMax; ++c )
  {
    m_Pages.pop_front();
  }
}

void CMainFrame::AddVisibleButNotLoaded()
{
  if ( m_Pages.empty() )
    return;

  bool bShowWarn = false;
  WTL::CRect r;
  WTL::CWaitCursor wc;
  while ( true )
  {
    int index = m_Pages.front()->GetPageIndex() - 1;
    if ( index < 0 )
      break;
    
    r = m_Pages.front()->GetRect();
    r.MoveToY( r.top - r.Height() - g_cBetweenPageGap );
    if ( !IsVisible( r ) )
      break;

    PagePtr p( new CPage( m_hWnd, m_pDjVuDoc, r, true, index ) );
    if ( !p->LoadBmpSync() )
    {
      bShowWarn = true;
      break;
    }
    // place correction if the loaded page has the different size
    WTL::CRect newR = p->GetRect();
    r = m_Pages.front()->GetRect();
    r.MoveToY( r.top - newR.Height() - g_cBetweenPageGap );
    int dy = r.top - newR.top;
    p->Scroll( 0, dy );

    m_Pages.push_front( p );
  }

  // ...
  while ( true )
  {
    int index = m_Pages.back()->GetPageIndex() + 1;
    if ( index >= m_pDjVuDoc->get_pages_num() )
      break;
    
    r = m_Pages.back()->GetRect();
    r.MoveToY( r.top + r.Height() + g_cBetweenPageGap );
    if ( !IsVisible( r ) )
      break;

    PagePtr p( new CPage( m_hWnd, m_pDjVuDoc, r, true, index ) );
    if ( !p->LoadBmpSync() )
    {
      bShowWarn = true;
      break;
    }
    m_Pages.push_back( p );
  }

  if ( bShowWarn )
  {
    ShowZoomWarning();
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

PagePtr CMainFrame::Get1stVisiblePage()
{
  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i )
  {
    if ( IsVisible( (*i)->GetRect() ) )
    {
      return *i;
    }
  }
  return PagePtr();
}

PagePtr CMainFrame::GetCurrentPage( int * pIndex /*= 0*/ )
{
  WTL::CRect clientRec;
  GetClientRect( &clientRec );
  WTL::CPoint clientCenter = clientRec.CenterPoint();
  
  PagePtr p1stVis;
  PagePtr p1st;

  int index1stVis = 0;
  int index1st = 0;
  int j=0;
  for ( Pages::iterator i=m_Pages.begin(); i!=m_Pages.end(); ++i, ++j )
  {
    WTL::CRect pgRect = (*i)->GetRect();
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
  m_tb.SetPages();
  int j=0;
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    m_mru[ i ] = L"";
    
    WTL::CString file;
    file.Format( L"MRU_file_%d", i );

    WTL::CString path;
    if ( !m_appInfo.Restore( path, (LPCWSTR)file ) &&
         !path.IsEmpty() )
    {
      bool bSkipAdding = false;
      for ( int k=j; k>=0; --k )
      {
        if ( !path.CompareNoCase( m_mru[ k ] ) )
        {
          bSkipAdding = true;
          break;
        }
      }
      if ( bSkipAdding )
      {
        DeleteMRURegVal( i );
        continue;
      }
      HANDLE h = ::CreateFile( path,
                               GENERIC_READ,
                               FILE_SHARE_READ,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               0 );
      if ( INVALID_HANDLE_VALUE == h )
      {
        DeleteMRURegVal( i );
        continue;
      }
      ::CloseHandle( h );
      
      m_mru[ j ] = path;
      ++j;
    }
  }

  bool bLoaded = false;
  if ( !m_cmdLine.IsEmpty() )
  {
    bLoaded = AppNewInstance( m_cmdLine );
  }

  CBookmarkInfo bm;
  CBookmarkDlg::LoadAutoBM( m_mru[ 0 ], bm );

  if ( !bLoaded && !m_mru[ 0 ].IsEmpty() )
  {
    OpenFile( m_mru[ 0 ], bm );
  }
}

void CMainFrame::SetCurFileInMru( WTL::CString const & fullFileName )
{
  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( !fullFileName.CompareNoCase( m_mru[ i ] ) )
    {
      WTL::CString t = m_mru[ i ];
      for ( int j=i; j>0; --j )
      {
        m_mru[ j ] = m_mru[ j-1 ];
      }
      m_mru[ 0 ] = t;
      return;
    }
  }

  int lastInd = g_cMruNumber-1;
  CBookmarkDlg::DoesAutoBMExistOnly( m_mru[ lastInd ] );
  for ( int i=lastInd; i>0; --i )
  {
    m_mru[ i ] = m_mru[ i-1 ];
  }  
  m_mru[ 0 ] = fullFileName;
}

void CMainFrame::DeleteMRURegVal( int index )
{
  CBookmarkDlg::DoesAutoBMExistOnly( m_mru[ index ] );
  WTL::CString file;
  file.Format( L"MRU_file_%d", index );
  m_appInfo.Delete( ATL::_U_STRINGorID(file) );
}

void CMainFrame::UpdateMruMenu()
{
  WTL::CMenuHandle mnu = (HMENU)::SendMessage( m_hWndCECommandBar, SHCMBM_GETMENU, 0, 0 );
  if ( !mnu )
    return;

  WTL::CMenuHandle sub0Mnu = mnu.GetSubMenu( 0 );
  ATLASSERT( sub0Mnu );
  if ( !sub0Mnu )
    return;

#pragma region MRU files menu finding
  WTL::CMenuHandle mruMnu;
  for ( int i = 0; true; ++i )
  {
    MENUITEMINFO mii = {0}; 
    mii.cbSize = sizeof mii;
    mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
    if ( !sub0Mnu.GetMenuItemInfo( i, true, &mii ) )
    {
      return;
    }
    if ( !mii.hSubMenu )
      continue;

    WTL::CMenuHandle mnu = mii.hSubMenu;
    if ( mnu.GetMenuItemInfo( 0, true, &mii ) && ID_FILE_MRU_FIRST == mii.wID )
    {
      mruMnu = mnu;
      break;
    }
  }
  ATLASSERT( mruMnu );
  if ( !mruMnu )
    return;
#pragma endregion

#pragma region Cleanup
  for ( int mCnt = ATL::GetMenuItemCount( mruMnu ); mCnt--; )
  {
    mruMnu.RemoveMenu( mCnt, MF_BYPOSITION );
  }
#pragma endregion

  for ( int i=0; i<g_cMruNumber; ++i )
  {
    if ( m_mru[ i ].IsEmpty() )
      continue;
 
    wchar_t fName[_MAX_PATH];
    _wsplitpath_s( (LPCWSTR)m_mru[ i ],
                   0,0,
                   0,0,
                   fName,sizeof(fName)/sizeof(fName[0]),
                   0,0 );
    mruMnu.AppendMenuW( 0, ID_FILE_MRU_FIRST+i, fName );
  }
}

LRESULT CMainFrame::OnMru( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL & bHandled )
{
  unsigned i = wID - ID_FILE_MRU_FIRST;
  if ( g_cMruNumber <= i || m_mru[ i ].IsEmpty() )
    return 1;

  CBookmarkInfo bm;
  CBookmarkDlg::LoadAutoBM( m_mru[ i ], bm );
  if ( OpenFile( m_mru[ i ], bm ) )
  {
    Invalidate();
  }
  else
  {
    DeleteMRURegVal( i );

    for ( unsigned j=i+1; j<g_cMruNumber; ++j )
    {
      m_mru[ j-1 ] = m_mru[ j ];
    }
    m_mru[ g_cMruNumber-1 ].Empty();
    UpdateMruMenu();
  }

  return 0;
}

bool CMainFrame::GoToPage( CBookmarkInfo const & bkmrk )
{
  Invalidate();
  WTL::CWaitCursor wc;
  PagePtr p1stVis = Get1stVisiblePage();
  m_Pages.clear();
  PagePtr pPage( new CPage( m_hWnd,
                            m_pDjVuDoc,
                            bkmrk.m_pageRect,
                            true,
                            bkmrk.m_pageIndex ) );
  if ( !pPage->LoadBmpSync() )
  {
    m_Pages.push_back( p1stVis );
    ShowZoomWarning(); // TODO: <- transfer to Loader and make adequate message composition.
    return false;
  }
    
  m_Pages.push_back( pPage );
  m_bDirty = true;
  PageLayout();

  return true;
}

LRESULT CMainFrame::OnNavigationGotopage( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  if ( !m_pDjVuDoc )
    return 0;

  CFullScrnOnOff fullScrOnOff( *this );

  PagePtr p1stVis = Get1stVisiblePage();
  int visPg = 1 + (p1stVis ? p1stVis->GetPageIndex() : 0 );
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

  WTL::CRect clientRect;
  if ( !GetClientRect( &clientRect ) )
  {
    return 0;
  }
  
  CBookmarkInfo bkmrk( pg, clientRect );
  GoToPage( bkmrk );

  return 0;
}

LRESULT CMainFrame::OnNavigationBackForward( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
{
  if ( m_history.size() == 1 )
  {
    return 0;
  }

  int ind = 0;
  int delta = m_histCurInd < m_histStartInd ? m_history.size() : 0;
  int c = m_histCurInd - m_histStartInd + delta;

  if ( ID_NAVIGATION_FORWARD == wID )
  {
    ind = c + 1;
  }
  else
  {
    ind = c - 1;
  }
  if ( ind < 0 || int(m_history.size()) <= ind  )
  {
    return 0;
  }

  ind = (ind + m_histStartInd) % m_history.size();

  m_histCurInd = ind;
  CBookmarkInfo bkmrk( m_history[ind] );
  GoToPage( bkmrk );

  return 0;
}

LRESULT CMainFrame::OnNavigationHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  WTL::CMenu m;
  if ( !m.CreatePopupMenu() )
  {
    return 0;
  }
    
  WTL::CString fmt;
  fmt.LoadString( IDS_HIST_MENUITEM ); // "Page %d"

  int cnt = m_history.size();
  if ( !cnt )
  {
    return 0;
  }
  for ( int i = 0; i < cnt; ++i )    
  {
    int ind = (i + m_histStartInd) % m_maxHistL;
    WTL::CString mstr;
    mstr.Format( fmt, m_history[ ind ].m_pageIndex+1 );
    m.AppendMenu( 0, ind+1, mstr );
  }
  
  m.CheckMenuItem( m_histCurInd+1, MF_BYCOMMAND | MF_CHECKED );

  POINT p = m_tb.GetPointForMenu();
  int mItem = -1 + m.TrackPopupMenu( TPM_RETURNCMD|TPM_BOTTOMALIGN|TPM_LEFTALIGN, p.x, p.y, m_hWnd );
  if ( mItem < 0  ||  m_history.size() == 1 )
  {
    return 0;
  }

  if ( GoToPage( m_history[ mItem ] ) )
  {
    m_histCurInd = mItem; // current history page
  }
  
  return 0;
}

void CMainFrame::FinishCtrl()
{
  if ( m_pCtrl )
  {
    UISetCheck(m_cmdIDCntrl, false);
    m_pCtrl.Reset();
    RemoveChainEntry( 0 );
  }
}

void CMainFrame::FinishCtrl( void * pSourceCtrl, bool bCancel )
{
  if ( pSourceCtrl != m_pCtrl || !m_pCtrl )
    return;
  
  if ( !bCancel )
  {
    CRectZoomCtrl * pZoomCtrl = dynamic_cast<CRectZoomCtrl*>( m_pCtrl.GetPtr() );
    if ( pZoomCtrl )
    {
      WTL::CRect r = pZoomCtrl->GetRect();
      CalcZoomKandOffset( r );
    }
  }
  FinishCtrl();
}

void CMainFrame::CalcZoomKandOffset( WTL::CRect & r )
{
  r.NormalizeRect();

  if ( r.Width() < g_cMinZoomRect && r.Height() < g_cMinZoomRect )
    return;
  
  WTL::CRect cr;
  if ( !GetClientRect( &cr ) )
    return;

  double rx = ((double)cr.Width())/r.Width();
  double ry = ((double)cr.Height())/r.Height();
  double zoomK  = (rx < ry ? rx : ry);

  if ( g_cZoomMaxK < zoomK )
    zoomK = g_cZoomMaxK;
  if ( zoomK < g_cZoomMinK )
    zoomK = g_cZoomMinK;
    
  WTL::CPoint clientCenter = cr.CenterPoint();
  WTL::CPoint newWinCenter = r.CenterPoint();

  PagePtr pCurPg = GetCurrentPage();
  m_Pages.clear();

  int index = pCurPg->GetPageIndex();
  WTL::CRect pR = pCurPg->GetRect();
  WTL::CPoint newImgCenter = newWinCenter - pR.TopLeft();

  pR.right  = Round(zoomK * pR.Width() );
  pR.bottom = Round(zoomK * pR.Height());
  pR.left   = 0;
  pR.top    = 0;

  newImgCenter.x   = Round(zoomK * newImgCenter.x);
  newImgCenter.y   = Round(zoomK * newImgCenter.y);

  WTL::CPoint v = clientCenter - newImgCenter;

  pR.left   += v.x;
  pR.right  += v.x;
  pR.top    += v.y;
  pR.bottom += v.y;
 
  WTL::CWaitCursor wc;
  Invalidate();
  PagePtr pNewPage( new CPage( m_hWnd, m_pDjVuDoc, pR, true, index ) );
  if ( !pNewPage->LoadBmpSync() )
  {
    m_Pages.push_back( pCurPg );
    ShowZoomWarning();
    return;
  }
 
  m_Pages.push_back( pNewPage );
  m_bDirty = true;
  PageLayout();
}

LRESULT CMainFrame::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
  DWORD t = GetTickCount();
  if ( t - m_1stClick < g_cDobleClickTime )
  {
    m_bFullScreen = !m_bFullScreen;
    UpdateScreenMode();
    return 0;
  }
  m_1stClick = t;

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

  WTL::CMenuHandle mnu;
  mnu.Attach( hMenu );
  WTL::CMenuHandle sub0Mnu = mnu.GetSubMenu( 0 );
  ATLASSERT( sub0Mnu );
  if ( !sub0Mnu )
    return 0;

  sub0Mnu.TrackPopupMenu( 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), m_hWnd );

  return 0;
}

LRESULT CMainFrame::OnTrayNotyfy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
{
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
  m_bFullScreen = !m_bFullScreen;
  UpdateScreenMode();

  return 0;
}

LRESULT CMainFrame::OnScrollByTap( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  OnCtrlButton< ID_SCROLL_BY_TAP, CScrollByTap >();
  return 0;
}
LRESULT CMainFrame::OnMoveByStylus( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  OnCtrlButton< ID_MOVE_BY_STYLUS, CMoveByStylus >();
  return 0;
}

WTL::CPoint CMainFrame::GetImgOrg( int pageIndex )
{
  ATLASSERT( !m_Pages.empty() );
  return m_Pages[ pageIndex ]->GetRect().TopLeft();
}

void CMainFrame::MoveImage( WTL::CPoint vec, int pageIndex )
{
  int m = vec.x;
  ScrollPagesHor( m );
  m = vec.y;
  ScrollPagesVert( m );
  RedrawWindow();
}

LRESULT CMainFrame::OnBookmark( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled )
{
  CFullScrnOnOff fs( *this );  
  int visPgInd = 0;
  WTL::CRect r;

  if ( m_pDjVuDoc )
  {
    PagePtr p1stVis = Get1stVisiblePage();
    if ( p1stVis )
    {
      visPgInd = p1stVis->GetPageIndex();
      r = p1stVis->GetRect();
    }
  }
  
  CBookmarkInfo bi( visPgInd, r );
  CBookmarkDlg dlg( m_pDjVuDoc ? m_mru[ 0 ] : (wchar_t const * )0, bi );
  if ( ID_GOTOBOOKMARK != dlg.DoModal() )
    return 0;
  
  WTL::CString szCurFullPathName;
  if ( dlg.GetGoToBookMark( bi, szCurFullPathName ) )
  {
    if ( !m_pDjVuDoc || szCurFullPathName.CompareNoCase( m_mru[ 0 ] ) )
    {
      OpenFile( szCurFullPathName, bi );
    }
    else
    {
      GoToPage( bi );
    }
  }

  return 0;
}

LRESULT CMainFrame::OnNofify_1( WORD /*wNotifyCode*/,WORD /*wID*/,HWND /*hWndCtl*/,BOOL& bHandled )
{
  NotificationRemove();

  return 0;
}
