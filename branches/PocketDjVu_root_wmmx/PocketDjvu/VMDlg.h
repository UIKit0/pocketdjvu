#pragma once

#include "resource.h"
#include "./RegVMValues.h"

class CVMDlg
    : public WTL::CPropertyPageImpl<CVMDlg>
    , public WTL::CDialogResize<CVMDlg>
    , public WTL::CWinDataExchange<CVMDlg>
{
    typedef WTL::CPropertyPageImpl<CVMDlg>  BaseWnd;
    typedef WTL::CDialogResize<CVMDlg>      BaseResize;
    typedef WTL::CWinDataExchange<CVMDlg>   BaseEx;

public:
    enum { IDD = IDD_VM };
    CVMDlg();
    ~CVMDlg();

    BEGIN_DLGRESIZE_MAP(CVMDlg)
        DLGRESIZE_CONTROL(IDC_LEVEL_PLACEHOLDER,  0)
        DLGRESIZE_CONTROL(IDC_ST1,                0)
        DLGRESIZE_CONTROL(IDC_ST2,                DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_ST3,                DLSZ_MOVE_X)

        DLGRESIZE_CONTROL(IDC_LEVEL, 0)
        DLGRESIZE_CONTROL(IDC_LEVEL_SLIDER,       DLSZ_SIZE_X)

        DLGRESIZE_CONTROL(IDC_SWAP_FILE,          DLSZ_SIZE_X) // (*)
        DLGRESIZE_CONTROL(IDC_RAM_ONLY,           DLSZ_SIZE_X|DLSZ_MOVE_X) // (*)

        DLGRESIZE_CONTROL(IDC_SWAPFILE_STATIC,    0/*DLSZ_SIZE_X*/)
        DLGRESIZE_CONTROL(IDC_SWAPFILE,           DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_BROWSE_PATH,        DLSZ_MOVE_X)

        DLGRESIZE_CONTROL(IDC_MBSIZE_STATIC,      0)
        DLGRESIZE_CONTROL(IDC_MBSIZE,             DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_MBSIZE_SPIN,        DLSZ_MOVE_X)

        DLGRESIZE_CONTROL(IDC_PERCENT,            0)
        DLGRESIZE_CONTROL(IDC_RAM_PERCENT_SLIDER, DLSZ_SIZE_X)

        DLGRESIZE_CONTROL(IDC_COMMENT,            DLSZ_SIZE_X/*|DLSZ_MOVE_Y*/)
    END_DLGRESIZE_MAP()

BEGIN_DDX_MAP(CVMDlg)
    DDX_TEXT_LEN(IDC_SWAPFILE,                    m_regVmValues.SwapFileName, MAX_PATH)
    DDX_UINT_RANGE(IDC_MBSIZE,                    m_regVmValues.SizeMB, DWORD(g_cSwapLowLimitMB), DWORD(g_cSwapUpperLimitMB))
    DDX_CONTROL_HANDLE(IDC_LEVEL_SLIDER,          m_LevelSlider)
    DDX_CONTROL_HANDLE(IDC_MBSIZE_SPIN,           m_spin)
    DDX_RADIO(IDC_SWAP_FILE,                      m_regVmValues.SwapOrRam)
    DDX_CONTROL_HANDLE(IDC_RAM_PERCENT_SLIDER,    m_ramPercent)
END_DDX_MAP()

BEGIN_MSG_MAP(CBookmarkDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_HANDLER(IDC_BROWSE_PATH, BN_CLICKED, OnBrowsePath)
    COMMAND_HANDLER(IDC_SWAP_FILE, BN_CLICKED, OnBnClickedSwapOrRAM)
    COMMAND_HANDLER(IDC_RAM_ONLY, BN_CLICKED, OnBnClickedSwapOrRAM)
    CHAIN_MSG_MAP(BaseResize)
    CHAIN_MSG_MAP(BaseWnd)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

    LRESULT OnInitDialog( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT OnBrowsePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    //... CWinDataExchange "overriding" ...
    void OnDataValidateError( UINT nCtrlID, BOOL bSave, _XData & data );

    //... CPropertyPageImpl "overriding" ...
    int OnApply();

private:
    void ToggleControlsByVmMode();

private:  
    siv_vm::CRegVMValues    m_regVmValues;
    WTL::CButton            m_radioSwap;
    WTL::CButton            m_radioRam;
    WTL::CTrackBarCtrl      m_LevelSlider;
    WTL::CUpDownCtrl        m_spin;
    WTL::CTrackBarCtrl      m_ramPercent;
public:
    LRESULT OnBnClickedSwapOrRAM(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
