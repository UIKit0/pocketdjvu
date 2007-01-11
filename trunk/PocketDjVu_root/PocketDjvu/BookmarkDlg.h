#pragma once

#include "resource.h"
#include "SIPState.h"

class CBookmarkDlg : public CStdDialogResizeImpl<CBookmarkDlg>
{
  typedef CStdDialogResizeImpl<CBookmarkDlg> Base;
public:
  enum { IDD = IDD_BOOKMARK };

	CBookmarkDlg( bool bAddBookmark = false );
	~CBookmarkDlg();

BEGIN_DLGRESIZE_MAP(CAboutDlg)
  //BEGIN_DLGRESIZE_GROUP()
    DLGRESIZE_CONTROL(IDC_STATIC_ADD, 0)
    DLGRESIZE_CONTROL(IDC_BOOKMARK_NAME, DLSZ_SIZE_X)
    DLGRESIZE_CONTROL(IDC_BTNADD, DLSZ_MOVE_X)
    DLGRESIZE_CONTROL(IDC_BTNDEL, DLSZ_MOVE_X)
  //END_DLGRESIZE_GROUP()

  //BEGIN_DLGRESIZE_GROUP()
    DLGRESIZE_CONTROL(IDC_TREE, DLSZ_SIZE_X|DLSZ_SIZE_Y)
  //END_DLGRESIZE_GROUP()
END_DLGRESIZE_MAP()

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

// DATA:
private:
  bool m_bAddBookmark;
  CSIPState m_sip;
};


