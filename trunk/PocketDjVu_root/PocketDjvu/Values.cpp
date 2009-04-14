#include "Stdafx.h"

#include "./Values.h"
#include "./BrowseSettings.h"

CValues::CRegBrowseValues CValues::m_regBrowseValues( HKEY_CURRENT_USER, APP_REG_PATH_BROWSE );
CValues::CRegMiscValues   CValues::m_regMiscValues( HKEY_CURRENT_USER, APP_REG_MISC );
static bool bInit = CValues::Init();

bool CValues::Init()
{
  bool ret   = ERROR_SUCCESS == m_regBrowseValues.Load();
  ret = ret && ERROR_SUCCESS == m_regMiscValues.Load();
  return ret;
}

void CValues::Assign( CValues::CRegBrowseValues const & set )
{
  m_regBrowseValues = set;
}

void CValues::Assign( CRegMiscValues const & set )
{
    m_regMiscValues = set;
}