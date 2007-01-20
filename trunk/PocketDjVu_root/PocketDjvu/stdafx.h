// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change this value to use different versions
#define WINVER UNDER_CE // SIV: 0x0420

#define _WIN32_WCE_AYGSHELL 1

#include <atlbase.h>
#include <atlstr.h>   // SIV:
#define _WTL_NO_CSTRING
namespace WTL
{
  typedef ::ATL::CString CString;
}

#include <atlapp.h>

extern WTL::CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlx.h>

#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")

#include <atlmisc.h>  // SIV:
#include <atlgdi.h>   // SIV:
#include <atlscrl.h>  // SIV:
#include <atlwince.h> // SIV:
#include <atlddx.h>   // SIV:
