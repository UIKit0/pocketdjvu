#pragma once

struct __declspec(novtable) ICtrlNotify
{
  virtual void FinishCtrl( void * pSourceCtrl, bool bCancel ) = 0;
};
