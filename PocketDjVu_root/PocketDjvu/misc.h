#pragma once

WTL::CString GetModuleVersionStr( HMODULE hModule );

void ShowNotification( HWND hwndSink, wchar_t const * szCaption, wchar_t const * szBodytext );
void NotificationRemove();

BOOL CreateDlgMenuBar( UINT nToolBarId, HWND hWndParent );