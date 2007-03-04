#pragma once

static wchar_t const * APP_REG_PATH    = L"Software\\landi-soft.com\\PocketDjVu";
static wchar_t const * APP_REG_PATH_VM = L"Software\\landi-soft.com\\PocketDjVu\\VM\\";

const float     g_cEps = 1.e-5;
const int       g_cPageGap = 4;
const float     g_cZoomK = 0.125f;
const float     g_cZoomMaxK = 5.f;
const float     g_cZoomMinK = .3f;
const int       g_cMinZoomRect = 16;
const float     g_cPageScrollK = 0.9f;
const int       g_cMruNumber = 7;
const int       g_cLongScrollTimeout = 1100;
const int       g_cShortScrollTimeout = 20;
const int       g_cPixelsInLineScroll = 2;
const int       g_cDobleClickTime = 700; // msec
const bool      g_cGUItoolBarUseWinCapBkgrnd = false;
const int       g_cCacheMax = 1; // Preserved invisible pages in cache (was 2).

static wchar_t const * SWAP_FILENAME = L"\\SD Card\\file.swp";
static wchar_t const * SWAP_NAME     = L"\\file.swp";
const unsigned  g_cSwapLowLimitMB = 16;
const unsigned  g_cSwapUpperLimitMB = 128;
const unsigned  g_cSwapDefaultMB = 64;

/// It's pixels for double click recognitions.
/// On my PPC 40 ones were returned by GetSystemMetrics(SM_CXDOUBLECLK) and it's unacceptable.
const int g_sDxDblClck = 6; 

/// Distance between toolbar buttons and our info pannel
const int g_cTollBarGap = 4;