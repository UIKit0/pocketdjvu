#include "StdAfx.h"

#include "./VMDlg.h"

TODO...
CVMDlg::CVMDlg() : m_storage( change it! HKEY_CURRENT_USER, L"Software\\My Company\\Application\\1.0")
{
  m_storage.Load();
}

CVMDlg::~CVMDlg()
{
}

bool CVMDlg::OnPageCreate()
{
  // No have created window here
  // true - allow page to be created, false - prevent creation
  return true;
}

LRESULT CVMDlg::OnInitDialog( UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandled )
{
  DlgResize_Init( false, false, 0 );

  return 0;
}

/* TODO:
- implement REGISTRY map for parameters;
- implement OnOK/OnCancel.
*/