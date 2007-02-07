#include "StdAfx.h"

#include "./VMDlg.h"

CVMDlg::CVMDlg()
{
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