#pragma once

#include <set>
#include <map>

#include "resource.h"
#include "SIPState.h"
#include "BookmarkInfo.h"
#include "smart_ptr.h"

//-----------------------------------------------------------------------------
class CBookmarkDlg :
  public WTL::CStdDialogResizeImpl<CBookmarkDlg>,
  public WTL::CWinDataExchange<CBookmarkDlg>
{
  typedef WTL::CStdDialogResizeImpl<CBookmarkDlg> Base;

public:
// TYPES:
  enum { IDD = IDD_BOOKMARK };

// METHODS: 
	CBookmarkDlg( wchar_t const * szFullPathName, CBookmarkInfo const & bookmarkInfo );

	~CBookmarkDlg();

  bool GetGoToBookMark( CBookmarkInfo & bm, WTL::CString & o_szCurFullPathName )
  {
    if ( !m_pGoToBM )
      return false;
    o_szCurFullPathName = m_szCurFullPathName;
    bm = *m_pGoToBM;
    return true;
  }

  static bool SaveAutoBM( wchar_t const * szFullPath, CBookmarkInfo const & rBM );
  static bool LoadAutoBM( wchar_t const * szFullPath, CBookmarkInfo & rBM );
  static void DoesAutoBMExistOnly( wchar_t const * szFullPath );

BEGIN_DLGRESIZE_MAP(CBookmarkDlg)
  DLGRESIZE_CONTROL(IDC_STATIC_ADD, 0)
  DLGRESIZE_CONTROL(IDC_BOOKMARK_NAME, DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_BTNADD, DLSZ_MOVE_X)
  DLGRESIZE_CONTROL(IDC_BTNDEL, DLSZ_MOVE_X)  
  
  DLGRESIZE_CONTROL(IDC_TREE, DLSZ_SIZE_X|DLSZ_SIZE_Y)
END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CBookmarkDlg)
  DDX_CONTROL_HANDLE(IDC_TREE, m_tree)
  DDX_TEXT(IDC_BOOKMARK_NAME, m_sBookmarkName)
END_DDX_MAP()

BEGIN_MSG_MAP(CBookmarkDlg)
  MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
  MESSAGE_HANDLER(WM_WININICHANGE, OnWininiChange)
  
  COMMAND_ID_HANDLER(IDCANCEL, OnCancell)
  COMMAND_ID_HANDLER(IDOK, OnSave)
  COMMAND_ID_HANDLER(ID_GOTOBOOKMARK, OnGotoBookmark)

  COMMAND_ID_HANDLER(IDC_BTNADD, OnBtnAdd)
  COMMAND_ID_HANDLER(IDC_BTNDEL, OnBtnDel)
  NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTvnSelchangedTree)
  NOTIFY_HANDLER(IDC_TREE, NM_DBLCLK, OnDblclkTree)
  NOTIFY_HANDLER(IDC_TREE, TVN_BEGINLABELEDIT, OnTvnBeginLabelEditTree)
  NOTIFY_HANDLER(IDC_TREE, TVN_ENDLABELEDIT, OnTvnEndLabelEditTree)
  MESSAGE_HANDLER(m_BookmarkDlgMsg,OnBookmarkDlgMsg)
  CHAIN_MSG_MAP(Base)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnCancell( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnSave( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnGotoBookmark( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnBtnAdd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnBtnDel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnTvnSelchangedTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
  LRESULT OnDblclkTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
  LRESULT OnTvnBeginLabelEditTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
  LRESULT OnTvnEndLabelEditTree(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled);
  LRESULT OnBookmarkDlgMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
// TYPES:
  class CBookmarkInfoRef : public siv_hlpr::CRefCntr<>, public CBookmarkInfo
  {
  public:
    CBookmarkInfoRef( CBookmarkInfo const & src, wchar_t const * szName ) : m_szName( szName )
    {
      *(CBookmarkInfo*)this = src;
    }

    wchar_t const * GetName() const
    {
      return m_szName;
    }

    void SetName( wchar_t const * szName )
    {
      m_szName = szName;
    }
  private:
    WTL::CString m_szName;
  };
  typedef siv_hlpr::CSimpSPtr<CBookmarkInfoRef> BMPtr;
  typedef std::set<BMPtr> BMSet;
  typedef std::map<WTL::CString,BMSet> BMs;

  void LoadFromRegistry();
  void LoadBookmark( WTL::CString szPathKey );
  void SaveToRegistry();
  void FindOrCreateCurrentFileBranch();
  void EnableGotoBookmarkMenu( bool bEnable );

// DATA:
private:
  CSIPState m_sip;
  ATL::CRegKey m_rootReg;

  bool m_bNotSaved;

  /// The full path to currently opened file.
  WTL::CString m_szCurFullPathName;
  /// Bokkmark data for currently opened file.
  CBookmarkInfo m_curBookmarkInfo;
  /// The bookmark creation is possible only in the current branch.
  WTL::CTreeItem m_currentFileItem;

  /// Root container of bookmarks.
  BMs m_bms;
  
  /// Menu bar of dialog.
  HWND m_hWndMenuBar;
  /// Bookmark tree-control.
  WTL::CTreeViewCtrlEx m_tree;
  /// Bookmark name which correesponds to edit-box.
  WTL::CString m_sBookmarkName;
  
  BMPtr m_pGoToBM;

  enum { MSG_NOTHING, MSG_SORTandSHOW };
  static UINT m_BookmarkDlgMsg;
};
