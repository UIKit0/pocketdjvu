#pragma once

static wchar_t const * APP_REG_PATH = L"Software\\landi-soft.com\\PocketDjVu";
const float g_cEps = 1.e-5;
const int   g_cPageGap = 4;
const float g_cZoomK = 0.125f;
const float g_cZoomMaxK = 5.f;
const float g_cZoomMinK = .3f;
const int   g_cMinZoomRect = 16;
const float g_cPageScrollK = 0.9f;
const int   g_cMruNumber = 7;
const int   g_cLongScrollTimeout = 1100;
const int   g_cShortScrollTimeout = 20;
const int   g_cPixelsInLineScroll = 2;
const int   g_cDobleClickTime = 700; // msec

/// It's pixels for double click recognitions.
/// On my PPC 40 ones were returned by GetSystemMetrics(SM_CXDOUBLECLK) and it's unacceptable.
const int   g_sDxDblClck = 6; 