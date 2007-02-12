#pragma once

#include "smart_ptr.h"

//-----------------------------------------------------------------------------
struct __declspec(novtable) ICtrlNotify
{
  virtual void FinishCtrl( void * pSourceCtrl, bool bCancel ) = 0;
};

//-----------------------------------------------------------------------------
class CControllerBase :
  public siv_hlpr::CRefCntr<>
  , public ATL::CMessageMap
{
public:
  CControllerBase( ICtrlNotify * pSubscriber ) : m_pSubscriber(pSubscriber)
  {
  }

public:
  BEGIN_MSG_MAP(CControllerBase)
    m_hWnd = hWnd; // SIV: small hack to get HWND without overriding
  END_MSG_MAP()

protected:
  HWND m_hWnd;
  ICtrlNotify * m_pSubscriber;
};

//-----------------------------------------------------------------------------
typedef siv_hlpr::CSimpSPtr<CControllerBase> ControllerPtr;
