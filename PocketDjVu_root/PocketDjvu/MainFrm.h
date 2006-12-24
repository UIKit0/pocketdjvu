#pragma once

#include <deque>

#include "../libdjvu/DjVuDocument.h"
#include "./Constants.h"
#include "./Page.h"
#include "./RectZoomCtrl.h"
#include "./CNofyIcon.h"

//-----------------------------------------------------------------------------
class CMainFrame : 
  public CFrameWindowImpl<CMainFrame>
  , public CUpdateUI<CMainFrame>
  , public CMessageFilter
  , public CIdleHandler
  , public CDoubleBufferImpl<CMainFrame>
  , public CAppWindowBase<CMainFrame>
  , public CDynamicChain
  , public ICtrlNotify
{  
  typedef std::deque< PagePtr > Pages;

public:
  DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

  CMainFrame();

  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual BOOL OnIdle();

  BEGIN_UPDATE_UI_MAP(CMainFrame)
    UPDATE_ELEMENT(ID_ZOOM_ZOOMBYRECT, UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
  END_UPDATE_UI_MAP()

  static UINT const WM_ICON_NOTIFICATION = WM_APP+100;

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
    CHAIN_MSG_MAP(CDoubleBufferImpl<CMainFrame>)
    CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
    CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    CHAIN_MSG_MAP(CAppWindowBase<CMainFrame>)
    MESSAGE_HANDLER(WM_ICON_NOTIFICATION,OnTrayNotyfy)
  END_MSG_MAP()

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
  LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnTrayNotyfy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  void DoPaint(CDCHandle dc);
  void AppSave();
  void LoadSettings();
  bool AppNewInstance( LPCTSTR lpstrCmdLine );
  void SetCmdLine( LPCTSTR lpstrCmdLine )
  {
    m_cmdLine = lpstrCmdLine;
  }

private:
  #pragma region ICtrlNotify
  void FinishCtrl( void * pSourceCtrl, bool bCancel );
#pragma endregion

  void UpdateScreenMode();
  void OnPageUpDn( bool bDown, bool bByPage = true  );
  void OnPageLeftRight( bool toRight, bool bByPage = true );
  bool IsVisible( CRect const & rect );
  void PageLayout( int moveY = 0 );
  void ScrollPagesVert( int & moveY );
  void ScrollPagesHor( int & moveX );
  void ClearRedundantCache();
  void AddVisibleButNotLoaded();
  bool OpenFile( LPCWSTR fullFileName, int pageIndex=0 );
  int Get1stVisiblePage();
  PagePtr GetCurrentPage( int * pIndex = 0 );  
  void SetCurFileInMru( CString const & fullFileName, int pageIndex );
  int GetPageIndFromMru( CString const & fileFullPath );
  void UpdateMruMenu();
  void RunTimerLong();
  void RunTimerShort();
  void StopTimer();
  void FinishZoomCtrl();
  void CalcZoomKandOffset( CRect & r );

  int Round( double val )
  {
    if ( val >= 0 )
    {
      return int( val + 0.5 );
    }

    return int( val - 0.5 );
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

  // DATA:
private:
#pragma region GUI
  bool m_bFullScreen;
  int m_curClientWidth;
  CString m_initDir;
  CString m_cmdLine;

  CAppInfoBase m_appInfo;
  
  // MRU
  struct CMru
  {
    CMru() : m_pageNum() {}
    void Clear()
    {
      m_curFillFileName.Empty();
      m_pageNum = 0;
    }

    CString m_curFillFileName;
    int m_pageNum;
  };
  CMru m_mru[ g_cMruNumber ];
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

#pragma endregion

#pragma region Controllers
  RectZoomCtrlPtr m_pZoomCtr;
#pragma endregion

#pragma region DjVu
  Pages m_Pages;
  GP<DjVuDocument> m_pDjVuDoc;
#pragma endregion  
};
