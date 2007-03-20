#pragma once

#include "resource.h"
#include "SIPState.h"
#include "BookmarkInfo.h"

//-----------------------------------------------------------------------------
class CBookmarkDlg :
  public WTL::CStdDialogResizeImpl<CBookmarkDlg, SHIDIF_CANCELBUTTON|SHIDIF_SIPDOWN|SHIDIF_SIZEDLGFULLSCREEN>,
  public WTL::CWinDataExchange<CBookmarkDlg>
{
  typedef WTL::CStdDialogResizeImpl<CBookmarkDlg, SHIDIF_CANCELBUTTON|SHIDIF_SIPDOWN|SHIDIF_SIZEDLGFULLSCREEN> Base;
public:
  enum { IDD = IDD_BOOKMARK };

	CBookmarkDlg( wchar_t const * szFullPathName, CBookmarkInfo const & bookmarkInfo );

	~CBookmarkDlg();

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
  COMMAND_ID_HANDLER(ID_SAVE, OnSave)
  COMMAND_ID_HANDLER(ID_GOTOBOOKMARK, OmGotoBookmark)

  COMMAND_ID_HANDLER(IDC_BTNADD, OnBtnAdd)
  COMMAND_ID_HANDLER(IDC_BTNDEL, OnBtnDel)
  NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTvnSelchangedTree)
  CHAIN_MSG_MAP(Base)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnCancell( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnSave( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OmGotoBookmark( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );

  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnBtnAdd( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnBtnDel( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnTvnSelchangedTree(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

private:
  void LoadFromRegistry();
  void FindOrCreateCurrentFileBranch();

// DATA:
private:
  CSIPState m_sip;
  ATL::CRegKey m_rootReg;

  WTL::CString m_szFullPathName;
  CBookmarkInfo m_bookmarkInfo;
  
  WTL::CTreeViewCtrlEx m_tree;
  WTL::CString m_sBookmarkName;

  WTL::CTreeItem m_currentFileItem;
};
