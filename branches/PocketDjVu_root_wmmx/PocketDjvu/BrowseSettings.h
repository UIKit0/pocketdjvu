#pragma once

#include "resource.h"
#include "./Values.h"

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
  
  DLGRESIZE_CONTROL(IDC_STATIC_LN1,DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_SCROLL_GRP,DLSZ_SIZE_X)

  DLGRESIZE_CONTROL(IDC_VERT_SCROLL_LAB,0)
  DLGRESIZE_CONTROL(IDC_VERT_SCROLL,DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_VERT_SPIN,DLSZ_MOVE_X)

  DLGRESIZE_CONTROL(IDC_HOR_SCROLL_LAB,0)
  DLGRESIZE_CONTROL(IDC_HOR_SCROLL,DLSZ_SIZE_X)
  DLGRESIZE_CONTROL(IDC_HOR_SPIN,DLSZ_MOVE_X)
END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CBrowseSettings)
  DDX_CONTROL_HANDLE(IDC_BROWSE_MODE_COMBO, m_comboMode)
  DDX_CONTROL_HANDLE(IDC_VERT_SPIN, m_spinVert)
  DDX_CONTROL_HANDLE(IDC_HOR_SPIN,  m_spinHor)

  DDX_UINT_RANGE(IDC_VERT_SCROLL, m_storage.pageScrollVertPercent, 5UL, 100UL)
  DDX_UINT_RANGE(IDC_HOR_SCROLL,  m_storage.pageScrollHorPercent,  5UL, 100UL)
  m_spinVert.SetRange( 5, 100 );
  m_spinHor.SetRange(  5, 100 );    
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
  CValues::CRegBrowseValues m_storage;
  WTL::CUpDownCtrl          m_spinVert;
  WTL::CUpDownCtrl          m_spinHor;
  WTL::CComboBox            m_comboMode;
};


