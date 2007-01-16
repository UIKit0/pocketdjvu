#pragma once

#include "resource.h"
#include "SIPState.h"

class CBookmarkDlg :
  public WTL::CStdDialogResizeImpl<CBookmarkDlg>,
  public WTL::CWinDataExchange<CBookmarkDlg>
{
  typedef WTL::CStdDialogResizeImpl<CBookmarkDlg> Base;
public:
  enum { IDD = IDD_BOOKMARK };

	CBookmarkDlg( wchar_t const * szFullPathName
                , int pageIndex
                , WTL::CRect const & pageRect
                , /*TODO: reserved*/bool bPortrait = true );

	~CBookmarkDlg();

BEGIN_DLGRESIZE_MAP(CAboutDlg)
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
  COMMAND_ID_HANDLER(IDOK, OnClickedOK)
  CHAIN_MSG_MAP(Base)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnClickedOK( WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled );
  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnWininiChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:
  void LoadFromRegistry();
  void FindOrCreateCurrentFileBranch();

// DATA:
private:
  WTL::CString m_szFullPathName;
  int m_pageIndex;
  WTL::CRect m_pageRect;
  bool m_bPortrait;

  CSIPState m_sip;
  ATL::CRegKey m_rootReg;

  WTL::CTreeViewCtrlEx m_tree;
  WTL::CString m_sBookmarkName;

  WTL::CTreeItem m_currentFileItem;
};


