#pragma once

#include "resource.h"
#include "./rsettings.h"
#include "./Constants.h"

class CVMDlg :
  public WTL::CPropertyPageImpl<CVMDlg>
  ,public WTL::CDialogResize<CVMDlg>
  ,public WTL::CWinDataExchange<CVMDlg>
{
  typedef WTL::CPropertyPageImpl<CVMDlg>  BaseWnd;
  typedef WTL::CDialogResize<CVMDlg>      BaseResize;
  typedef WTL::CWinDataExchange<CVMDlg>   BaseEx;

public:
  enum { IDD = IDD_VM };
  CVMDlg();
  ~CVMDlg();

BEGIN_DLGRESIZE_MAP(CVMDlg)
  DLGRESIZE_CONTROL(IDC_LEVEL_PLACEHOLDER,0)
  DLGRESIZE_CONTROL(IDC_ST1,              0)
  DLGRESIZE_CONTROL(IDC_ST2,              DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_ST3,              DLSZ_MOVE_X)

  DLGRESIZE_CONTROL(IDC_LEVEL, 0)
  DLGRESIZE_CONTROL(IDC_LEVEL_SLIDER,     DLSZ_SIZE_X)

  DLGRESIZE_CONTROL(IDC_SWAPFILE_STATIC,  DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_SWAPFILE,         DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_BROWSE_PATH,      DLSZ_MOVE_X)

  DLGRESIZE_CONTROL(IDC_MBSIZE_STATIC,    0)
  DLGRESIZE_CONTROL(IDC_MBSIZE,           DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_MBSIZE_SPIN,      DLSZ_MOVE_X)  
END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CVMDlg)
  DDX_TEXT_LEN(IDC_SWAPFILE,            m_storage.SwapFileName, MAX_PATH)
  DDX_UINT_RANGE(IDC_MBSIZE,            m_storage.SizeMB, DWORD(g_cSwapLowLimitMB), DWORD(g_cSwapUpperLimitMB))
  DDX_CONTROL_HANDLE(IDC_LEVEL_SLIDER,  m_LevelSlider)
  DDX_CONTROL_HANDLE(IDC_MBSIZE_SPIN,   m_spin)
END_DDX_MAP()

BEGIN_MSG_MAP(CBookmarkDlg)
  MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
  COMMAND_HANDLER(IDC_BROWSE_PATH, BN_CLICKED, OnBrowsePath)
  CHAIN_MSG_MAP(BaseResize)
  CHAIN_MSG_MAP(BaseWnd)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
  LRESULT OnBrowsePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  //... CWinDataExchange "overridings" ...
  void OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data );

  //... CPropertyPageImpl "overridings" ...
  int OnApply();

private:
  class CRegStorage : public CRegSettings
  {
  public:
    DWORD SizeMB;
    WTL::CString SwapFileName;
    DWORD Level;

    BEGIN_REG_MAP( CRegStorage )
      REG_ITEM( SizeMB, g_cSwapDefaultMB )
      REG_ITEM( SwapFileName, SWAP_FILENAME )
      REG_ITEM( Level, 0 )
    END_REG_MAP()
  };

  CRegStorage         m_storage;
  WTL::CTrackBarCtrl  m_LevelSlider;
  WTL::CUpDownCtrl    m_spin;  
};
