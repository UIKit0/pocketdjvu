// PocketDjvu.cpp : main source file for PocketDjvu.exe
//

#include "stdafx.h"

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run( LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWNORMAL )
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

  if ( lpstrCmdLine && *lpstrCmdLine )
  {
    wndMain.SetCmdLine( lpstrCmdLine );
  }
  wndMain.LoadSettings();
	wndMain.ShowWindow( nCmdShow );

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

// I have to create it because by default the new operator returns 0.
int __cdecl NewHandler( size_t sz )
{
  throw std::bad_alloc( "Not enought memory." );
  return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
#ifndef SIV_WAS_FIXED_ACTIVATEPREVIOUSINSTANCE
#   error ( "See this article: http://rsdn.ru/forum/Message.aspx?mid=2119153&only=1" )
#endif // SIV_WAS_FIXED_ACTIVATEPREVIOUSINSTANCE
	HRESULT hRes = CMainFrame::ActivatePreviousInstance(hInstance,lpstrCmdLine,false);
	if(FAILED(hRes) || S_FALSE == hRes)
	{
		return hRes;
	}

  // SIV: set the new operator and malloc function to throw bad_alloc.
  _set_new_handler( NewHandler );
  _set_new_mode(1);

	hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// Calling AtlInitCommonControls is not necessary to utilize picture,
	// static text, edit box, group box, button, check box, radio button, 
	// combo box, list box, or the horizontal and vertical scroll bars.
	// Calling AtlInitCommonControls with 0 is required to utilize the spin, 
	// progress, slider, list, tree, and tab controls.
	// Adding the ICC_DATE_CLASSES flag is required to initialize the 
	// date time picker and month calendar controls.
	// Add additional flags to support additoinal controls not mentioned above.
	AtlInitCommonControls( ICC_BAR_CLASSES 
						 | ICC_COOL_CLASSES
						 | ICC_PROGRESS_CLASS
						 | ICC_TOOLTIP_CLASSES
						 );
  BOOL bRes = SHInitExtraControls();
  ATLASSERT( bRes );

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
