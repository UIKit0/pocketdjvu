#include "Stdafx.h"

#include "./Values.h"
#include "./BrowseSettings.h"

CValues::CRegBrowseValues CValues::m_regBrowseValues( HKEY_CURRENT_USER, APP_REG_PATH_BROWSE );
static bool bInit = CValues::Init();

bool CValues::Init()
{
  bool ret = ERROR_SUCCESS == m_regBrowseValues.Load();
  return ret;
}

void CValues::Assign( CValues::CRegBrowseValues const & set )
{
  m_regBrowseValues = set;
}