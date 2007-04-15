#pragma once

static wchar_t const * APP_REG_PATH    = L"Software\\landi-soft.com\\PocketDjVu";
static wchar_t const * APP_REG_PATH_VM = L"Software\\landi-soft.com\\PocketDjVu\\VM\\";

const float     g_cEps = 1.e-5;
const int       g_cBetweenPageGap = 4;
const float     g_cZoomK = 0.125f;
const float     g_cZoomMaxK = 5.f;
const float     g_cZoomMinK = .3f;
const int       g_cMinZoomRect = 16;
const int       g_cPageScrollKPercent = 90;
const int       g_cHorizPanaramPercents = 80;
const int       g_cMruNumber = ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST;
const int       g_cLongScrollTimeout = 1100;  // msec
const int       g_cShortScrollTimeout = 20;  // msec
const int       g_cPixelsInLineScroll = 2;
const int       g_cDobleClickTime = 700; // msec
const bool      g_cGUItoolBarUseWinCapBkgrnd = false;
const int       g_cCacheMax = 1; // Preserved invisible pages in cache.

static wchar_t const * SWAP_FILENAME = L"\\SD Card\\file.swp";
static wchar_t const * SWAP_NAME     = L"\\file.swp";
const unsigned  g_cSwapLowLimitMB = 16;
const unsigned  g_cSwapUpperLimitMB = 128;
const unsigned  g_cSwapDefaultMB = 64;

/// It's pixels for double click recognitions.
/// On my PPC 40 ones were returned by GetSystemMetrics(SM_CXDOUBLECLK) and it's unacceptable.
/// And I don't like to change settings on system level.
const int g_sDxDblClck = 6; 

/// Toolbar metrics.
const int g_cToolBarOuterGap = 4;
const int g_cToolBarInnerGap = 3;
const int g_cPgFontHpx  = 14;
const int g_cGapBetweenRectAndInfoZone = 1;

const int g_cHistoryLength = 11;

static wchar_t const * BOOKMARK_REG_KEY       = L"\\Bookmarks";
static wchar_t const * BOOKMARK_REG_AUTOSAVE  = L"AutoSave";