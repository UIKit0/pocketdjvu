#pragma once

#include "./Values.h"

namespace siv_vm
{

    class CRegVMValues : public CRegSettings
    {
    public:
        DWORD           SizeMB;
        WTL::CString    SwapFileName;
        DWORD           Level;
        /// 0 - use swap file, 1 - use only Virtual RAM
        enum { USE_SWAP, USE_RAM_ONLY };
        int             SwapOrRam;
        DWORD           RamPercent;

        BEGIN_REG_MAP( CRegVMValues )
            REG_ITEM( SizeMB, g_cSwapLowLimitMB )
            REG_ITEM( SwapFileName, SWAP_FILENAME )
            REG_ITEM( Level, 0 )
            REG_ITEM( SwapOrRam, 1 )
            REG_ITEM( RamPercent, g_cVmRAMDefaultPercentage )
        END_REG_MAP()
    };

}
