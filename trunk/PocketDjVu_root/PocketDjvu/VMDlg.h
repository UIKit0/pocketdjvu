#pragma once

#include "resource.h"

class CVMDlg :
  public WTL::CPropertyPageImpl<CVMDlg>
  ,public WTL::CDialogResize<CVMDlg>
  ,public WTL::CWinDataExchange<CVMDlg>
{
typedef WTL::CDialogResize<CVMDlg> BaseResize;

public:
  enum { IDD = IDD_VM };
  CVMDlg();
  ~CVMDlg();

BEGIN_DLGRESIZE_MAP(CVMDlg)
  DLGRESIZE_CONTROL(IDC_LEVEL, 0)
  DLGRESIZE_CONTROL(IDC_LEVEL_SLIDER,     DLSZ_SIZE_X)

  DLGRESIZE_CONTROL(IDC_SWAPFILE_STATIC,  DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_SWAPFILE,         DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_BROWSE_PATH,                 DLSZ_MOVE_X)

  DLGRESIZE_CONTROL(IDC_MBSIZE_STATIC,    0)
  DLGRESIZE_CONTROL(IDC_MBSIZE,           DLSZ_SIZE_X)
END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CVMDlg)
  //DDX_CONTROL_HANDLE(IDC_TREE, m_tree)
  //DDX_TEXT(IDC_BOOKMARK_NAME, m_sBookmarkName)
END_DDX_MAP()

BEGIN_MSG_MAP(CBookmarkDlg)
  MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
  CHAIN_MSG_MAP(BaseResize)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

  bool OnPageCreate();
  //int OnSetActive();
  //BOOL OnKillActive();
  //int OnApply();

};
