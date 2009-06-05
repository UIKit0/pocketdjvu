#pragma once

#include "resource.h"
#include "./Values.h"

//------------------------------------------------------------------------------
class CMiscSettings :
    public WTL::CPropertyPageImpl<CMiscSettings>,
    public WTL::CDialogResize<CMiscSettings>,
    public WTL::CWinDataExchange<CMiscSettings>
{
    typedef WTL::CPropertyPageImpl<CMiscSettings>  BaseWnd;
    typedef WTL::CDialogResize<CMiscSettings>      BaseResize;
    typedef WTL::CWinDataExchange<CMiscSettings>   BaseEx;

public:
    // TYPES:
    enum { IDD = IDD_MISC_SETTINGS };

    CMiscSettings();
    ~CMiscSettings();

    BEGIN_DLGRESIZE_MAP(CMiscSettings)
        DLGRESIZE_CONTROL(IDC_SHOW_TRAY_ICON,   DLSZ_SIZE_X)

        DLGRESIZE_CONTROL(IDC_COMMENT,          DLSZ_SIZE_X|DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_DDX_MAP(CMiscSettings)
        DDX_CHECK(IDC_SHOW_TRAY_ICON, m_storage.m_bShowTrayIcon)
    END_DDX_MAP()

        BEGIN_MSG_MAP(CMiscSettings)
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
    CValues::CRegMiscValues m_storage;
    
    //WTL::CComboBox            m_comboMode;
};
