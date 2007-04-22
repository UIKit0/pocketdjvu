#pragma once

#include "resource.h"
#include "./rsettings.h"
#include "./Constants.h"

//------------------------------------------------------------------------------
class CBrowseSettings :
  public WTL::CPropertyPageImpl<CBrowseSettings>,
  public WTL::CDialogResize<CBrowseSettings>,
  public WTL::CWinDataExchange<CBrowseSettings>
{
  typedef WTL::CPropertyPageImpl<CBrowseSettings>  BaseWnd;
  typedef WTL::CDialogResize<CBrowseSettings>      BaseResize;
  typedef WTL::CWinDataExchange<CBrowseSettings>   BaseEx;

public:
  // TYPES:
  enum { IDD = IDD_BROWSE };

  CBrowseSettings();
	~CBrowseSettings();

BEGIN_DLGRESIZE_MAP(CBrowseSettings)
  DLGRESIZE_CONTROL(IDC_BROWSE_MODE_LAB,0)
  DLGRESIZE_CONTROL(IDC_BROWSE_MODE_COMBO,DLSZ_SIZE_X)

  DLGRESIZE_CONTROL(IDC_SCROLL_GRP,DLSZ_SIZE_X)

  DLGRESIZE_CONTROL(IDC_VERT_SCROLL_LAB,0)
  DLGRESIZE_CONTROL(IDC_VERT_SCROLL,DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_VERT_SPIN,DLSZ_MOVE_X)

  DLGRESIZE_CONTROL(IDC_HOR_SCROLL_LAB,0)
  DLGRESIZE_CONTROL(IDC_HOR_SCROLL,DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_HOR_SPIN,DLSZ_MOVE_X)
END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CBrowseSettings)
  DDX_UINT_RANGE(IDC_BROWSE_MODE_COMBO, m_storage.m_browseMode, CRegStorage::DEF_MODE, CRegStorage::PARROT_MODE)
  DDX_UINT_RANGE(IDC_VERT_SCROLL, m_storage.m_pageScrollVertPercent, 5U, 100U)
  DDX_UINT_RANGE(IDC_HOR_SCROLL,  m_storage.m_pageScrollHorPercent,  5U, 100U)
  
  DDX_CONTROL_HANDLE(IDC_BROWSE_MODE_COMBO, m_comboMode)
  DDX_CONTROL_HANDLE(IDC_VERT_SCROLL, m_spinVert)
  DDX_CONTROL_HANDLE(IDC_HOR_SCROLL,  m_spinHor)
END_DDX_MAP()

BEGIN_MSG_MAP(CBrowseSettings)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
  CHAIN_MSG_MAP(BaseResize)
  CHAIN_MSG_MAP(BaseWnd)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

  //... CWinDataExchange "overridings" ...
  void OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data );

  //... CPropertyPageImpl "overridings" ...
  int OnApply();

private:
  class CRegStorage : public CRegSettings
  {
  public:
    enum BROWSE_MODE { DEF_MODE, PARROT_MODE };
    BROWSE_MODE m_browseMode;
    unsigned m_pageScrollVertPercent;
    unsigned m_pageScrollHorPercent;    
    //WTL::CString SwapFileName;
    //DWORD Level;

    BEGIN_REG_MAP( CRegStorage )
      //REG_ITEM( SizeMB, g_cSwapDefaultMB )
      //REG_ITEM( SwapFileName, SWAP_FILENAME )
      //REG_ITEM( Level, 0 )
    END_REG_MAP()
  };

  CRegStorage         m_storage;
  WTL::CUpDownCtrl    m_spinVert;
  WTL::CUpDownCtrl    m_spinHor;
  WTL::CComboBox      m_comboMode;
};


