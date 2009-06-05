#include "StdAfx.h"

#include "./OptionsDlg.h"
#include "./misc.h"

COptionsDlg::COptionsDlg()
{
    BOOL res = TRUE;

    res = AddPage( m_BrowsePg ) && res;
    res = AddPage( m_MiscPg ) && res;

#if 0x0500 <= WINVER
    OSVERSIONINFO ovi = {0};
    ovi.dwOSVersionInfoSize = sizeof ovi;
    if ( GetVersionEx( &ovi ) && 5 <= ovi.dwMajorVersion )
    {
        res = AddPage( m_VMPg ) && res;
    }
#endif

    //res = AddPage( m_TimingPg ) && res;    
}

void COptionsDlg::OnSheetInitialized()
{
    ::CreateDlgMenuBar( IDR_MENU_OKCANCEL, m_hWnd );
}
