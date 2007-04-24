#pragma once

#include <deque>
#include <vector>

#include "../libdjvu/DjVuDocument.h"
#include "./Values.h"
#include "./Page.h"
#include "./RectZoomCtrl.h"
#include "./CNofyIcon.h"
#include "./DjVuToolBar.h"
#include "./BookmarkInfo.h"

//-----------------------------------------------------------------------------
class CMainFrame : 
  public WTL::CFrameWindowImpl<CMainFrame>
  , public WTL::CUpdateUI<CMainFrame>
  , public WTL::CMessageFilter
  , public WTL::CIdleHandler
  , public WTL::CDoubleBufferImpl<CMainFrame>
  , public WTL::CAppWindowBase<CMainFrame>
  , public ATL::CDynamicChain
  , public ICtrlNotify
{  
  typedef std::deque< PagePtr > Pages;

public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
  CMainFrame();

  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL OnIdle();

#pragma region UI update map
  BEGIN_UPDATE_UI_MAP(CMainFrame)
    UPDATE_ELEMENT(ID_ZOOM_ZOOMBYRECT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_SCROLL_BY_TAP,   UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_MOVE_BY_STYLUS,  UPDUI_MENUPOPUP | UPDUI_TOOLBAR)

    UPDATE_ELEMENT(ID_NAVIGATE_ADDBOOKMARK,  UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_NAVIGATION_GOTOPAGE,   UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_NAVIGATION_BACK,       UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_NAVIGATION_FORWARD,    UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_NAVIGATION_HISTORY,    UPDUI_MENUPOPUP | UPDUI_TOOLBAR)

    UPDATE_ELEMENT(ID_ZOOM_ZOOMIN,           UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_ZOOM_ZOOMOUT,          UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_ZOOM_FITSCREENWIDTH,   UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_ZOOM_FITSCREENHEIGHT,  UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
    UPDATE_ELEMENT(ID_ZOOM_FITPAGE,          UPDUI_MENUPOPUP | UPDUI_TOOLBAR)

    UPDATE_ELEMENT(ID_FULLSCREEN,            UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()
#pragma endregion UI update map

#pragma region Messaging
  static UINT const WM_ICON_NOTIFICATION = WM_APP+100;

#pragma region Message handlers map
  BEGIN_MSG_MAP(CMainFrame)
    CHAIN_MSG_MAP_DYNAMIC(0)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
    MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
    MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
    MESSAGE_HANDLER(WM_TIMER, OnTimer)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)

    COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
    COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
    COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
    COMMAND_ID_HANDLER(ID_TOOLS_OPTIONS, OnToolsOptions)
    
    COMMAND_RANGE_HANDLER(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE1+g_cMruNumber-1, OnMru)
    COMMAND_RANGE_HANDLER(ID_ZOOM_ZOOMIN, ID_ZOOM_FITPAGE, OnZoomCmd)
    
    COMMAND_ID_HANDLER(ID_NAVIGATION_GOTOPAGE, OnNavigationGotopage)
    COMMAND_RANGE_HANDLER(ID_NAVIGATION_BACK, ID_NAVIGATION_FORWARD, OnNavigationBackForward)
    COMMAND_ID_HANDLER(ID_NAVIGATION_HISTORY, OnNavigationHistory)
    
    COMMAND_ID_HANDLER(ID_FULLSCREEN, OnFullscreenCmd)
    COMMAND_ID_HANDLER(ID_SCROLL_BY_TAP, OnScrollByTap)
    COMMAND_ID_HANDLER(ID_MOVE_BY_STYLUS, OnMoveByStylus)
    COMMAND_ID_HANDLER(ID_NAVIGATE_BOOKMARK, OnBookmark)
    COMMAND_ID_HANDLER(ID_NOFIFY_1, OnNofify_1)    
    
    CHAIN_MSG_MAP(CDoubleBufferImpl<CMainFrame>)
    CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
    CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    CHAIN_MSG_MAP(CAppWindowBase<CMainFrame>)
    
    MESSAGE_HANDLER(WM_ICON_NOTIFICATION,OnTrayNotyfy)    
  END_MSG_MAP()
#pragma endregion 

#pragma region Message handlers 
  // Handler prototypes (uncomment arguments if needed):
  //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
  //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnToolsOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnSettingChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnZoomCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnMru(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnNavigationGotopage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnNavigationBackForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnNavigationHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnFullscreenCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnScrollByTap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnMoveByStylus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnTrayNotyfy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnBookmark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);  
  LRESULT OnNofify_1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
#pragma endregion 

#pragma endregion Messaging

  void DoPaint( WTL::CDCHandle dc );
  void AppSave();
  void LoadSettings();
  bool AppNewInstance( LPCTSTR lpstrCmdLine );
  void SetCmdLine( LPCTSTR lpstrCmdLine )
  {
    m_cmdLine = lpstrCmdLine;
  }

  void PageLayout( int moveY = 0 );
  WTL::CPoint GetImgOrg( int pageIndex );
  void MoveImage( WTL::CPoint vec, int pageIndex );

private:
#pragma region ICtrlNotify
  void FinishCtrl( void * pSourceCtrl, bool bCancel );
#pragma endregion

#pragma region Private worker methods
  void LoadTooltipStr( DWORD id );
  void UpdateScreenMode();
  void OnPageUpDn( bool bDown, bool bByPage = true  );
  void OnPageLeftRight( bool toRight, bool bByPage = true );
  bool IsVisible( WTL::CRect const & rect );
  void ScrollPagesVert( int & moveY );
  void ScrollPagesHor( int & moveX );
  void ClearRedundantCache();
  void AddVisibleButNotLoaded();
  bool OpenFile( LPCWSTR fullFileName, CBookmarkInfo bm );
  PagePtr Get1stVisiblePage();
  PagePtr GetCurrentPage( int * pIndex = 0 );  
  void SetCurFileInMru( WTL::CString const & fullFileName );
  void UpdateMruMenu();
  void DeleteMRURegVal( int index );
  void RunTimerLong();
  void RunTimerShort();
  void StopTimer();
  void FinishCtrl();
  void CalcZoomKandOffset( WTL::CRect & r );
  void ShowZoomWarning();
  void HistoryCleanup();  
  void UpdateHistory( PagePtr p1sVisPg );
  bool GoToPage( CBookmarkInfo const & bkmrk );


  template <int ID, typename TCtrl>
  void OnCtrlButton()
  {
    TCtrl * pCtrl = 0;
    if ( m_pCtrl )
    {
      pCtrl = dynamic_cast<TCtrl*>( m_pCtrl.GetPtr() );
      FinishCtrl();
    }

    if ( !pCtrl )
    {
      m_pCtrl.Reset( new TCtrl( this ) );
      m_cmdIDCntrl = ID;
      SetChainEntry( 0, m_pCtrl.GetPtr() );
      UISetCheck(ID, true);
    }
  }  

  class CFullScrnOnOff
  {
  public:
    CFullScrnOnOff( CMainFrame & rThis ) : m_rThis(rThis)
    {
      m_bFullScreen = m_rThis.m_bFullScreen;
      m_rThis.m_bFullScreen = false;
      m_rThis.UpdateScreenMode();
    }

    ~CFullScrnOnOff()
    {
      m_rThis.m_bFullScreen = m_bFullScreen;
      m_rThis.UpdateScreenMode();
    }

  private:
    CMainFrame & m_rThis;
    bool m_bFullScreen;
  };
#pragma endregion Private worker methods
  // DATA:
private:
#pragma region GUI
  std::vector<wchar_t const *> m_toolTips;

  bool m_bFullScreen;
  int m_curClientWidth;
  WTL::CString m_initDir;
  WTL::CString m_cmdLine;
  DWORD m_1stClick;

  WTL::CAppInfoBase m_appInfo;

  typedef std::vector<CBookmarkInfo> HistCont;
  HistCont m_history;
  int m_histStartInd;
  int m_histCurInd;
  int const m_maxHistL;

  // MRU
  WTL::CString  m_mru[ g_cMruNumber ];
  /// If true it means that something was changed in the settings and it worth to save.
  bool m_bDirty;

  /** FSM page scrolling scheme.
	\dot
	digraph SCROLL_STATE {
		node [shape=circle, fontname=Helvetica, fontsize=7];  
		start [ shape=doublecircle, label="SC_START", style=filled, fillcolor=lightgrey ];
		dn [ label="SC_DN"];
		l_timer [ label="SC_L_TIMER" ];
    sh_timer [ label="SC_SH_TIMER" ];
      
		start -> dn [ label=" key is pressed, run long timer" ];
		dn -> l_timer [ label=" timer was created" ];
    l_timer -> start [ label=" key is released, scroll 1 page, kill timer" ];
		l_timer -> sh_timer [ label=" long timeout elapsed, start short timer, scroll 1 pixel only" ];
    sh_timer -> sh_timer [ label=" short timeout elapsed, restart short timer, scroll 1 pixel only" ];
    sh_timer -> start [ label=" key is released, kill timer" ];		
	}
	\enddot
 */
  enum SCROLL_STATE { SC_START, SC_DN, SC_L_TIMER, SC_SH_TIMER };
  SCROLL_STATE m_scSate;
  UINT m_timerID;
  WPARAM m_cursorKey;
  
  CNofyIcon m_notyfyIcon;
  CDjVuToolBar m_tb;

#pragma endregion

#pragma region Controllers
  ControllerPtr m_pCtrl;
  int m_cmdIDCntrl;
#pragma endregion

#pragma region DjVu
  Pages m_Pages;
  GP<DjVuDocument> m_pDjVuDoc;
#pragma endregion
};
