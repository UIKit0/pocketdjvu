#include "Stdafx.h"

#include "./Values.h"
#include "./BrowseSettings.h"

CValues::CRegBrowseValues CValues::m_regBrowseValues;

void CValues::Assign( CValues::CRegBrowseValues const & set )
{
  m_regBrowseValues = set;
}