#pragma once

#include "./rsettings.h"

static wchar_t const * APP_REG_PATH    = L"Software\\Igor.Solovyov\\PocketDjVu";
static wchar_t const * APP_REG_PATH_VM = L"Software\\Igor.Solovyov\\PocketDjVu\\VM\\";
static wchar_t const * APP_REG_PATH_BROWSE = L"Software\\Igor.Solovyov\\PocketDjVu\\Browse\\";
static wchar_t const * APP_REG_MISC = L"Software\\Igor.Solovyov\\PocketDjVu\\Misc\\";

const float     g_cEps = 1.e-5;
const int       g_cBetweenPageGap = 4;
const float     g_cZoomK = 0.125f;
const float     g_cZoomMaxK = 5.f;
const float     g_cZoomMinK = .3f;
const int       g_cMinZoomRect = 16;
const int       g_cPageScrollVertPercent = 90;
const int       g_cPageScrollHorPercent = 80;
const int       g_cMruNumber = ID_FILE_MRU_LAST - ID_FILE_MRU_FIRST;
const int       g_cLongScrollTimeout = 1100;  // msec
const int       g_cShortScrollTimeout = 20;  // msec
const int       g_cPixelsInLineScroll = 2;
const int       g_cDobleClickTime = 700; // msec
const bool      g_cGUItoolBarUseWinCapBkgrnd = false;
const int       g_cCacheMax = 1; // Preserved invisible pages in cache.

static wchar_t const * SWAP_FILENAME = L"\\SD Card\\file.swp";
static wchar_t const * SWAP_NAME     = L"\\file.swp";
const unsigned  g_cSwapLowLimitMB = 64;
const unsigned  g_cSwapUpperLimitMB = 256;
const unsigned  g_cSwapDefaultMB = 92;
const unsigned  g_cVmRAMLowPercentage = 10;
const unsigned  g_cVmRAMHighPercentage = 90;

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

//------------------------------------------------------------------------------
class CValues
{
// TYPES:
public:
  class CRegBrowseValues : public CRegSettings
  {
  public:
    DWORD browseMode;
    DWORD pageScrollVertPercent;
    DWORD pageScrollHorPercent;

    BEGIN_REG_MAP( CRegBrowseValues )
      REG_ITEM( browseMode, CValues::DEF_MODE)
      REG_ITEM( pageScrollVertPercent, g_cPageScrollVertPercent )
      REG_ITEM( pageScrollHorPercent,  g_cPageScrollHorPercent )
    END_REG_MAP()
  };

  class CRegMiscValues : public CRegSettings
  {
  public:
    BOOL m_bShowTrayIcon;

    BEGIN_REG_MAP( CRegMiscValues  )
      REG_ITEM( m_bShowTrayIcon, TRUE)
    END_REG_MAP()
  };

public:
  static bool Init();

  /// @{
  enum BROWSE_MODE { CValues::DEF_MODE, CValues::PARROT_MODE };
  static BROWSE_MODE GetBrowseMode()
  {
    return BROWSE_MODE(m_regBrowseValues.browseMode);
  }
  static unsigned GetPageScrollVertPercent()
  {
    return m_regBrowseValues.pageScrollVertPercent;
  }
  static unsigned GetPageScrollHorPercent()
  {
    return m_regBrowseValues.pageScrollHorPercent;
  }
  /// @}

  /// @{
  static CRegMiscValues const & GetMiscValues()
  {
      return m_regMiscValues;
  }
  /// @}

  static void Assign( CRegBrowseValues const & set );
  static void Assign( CRegMiscValues const & set );

private:
  static CRegBrowseValues m_regBrowseValues;
  static CRegMiscValues   m_regMiscValues;
};