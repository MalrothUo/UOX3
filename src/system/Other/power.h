#ifndef __POWER_H__
#define __POWER_H__

namespace UOX
{

//o--------------------------------------------------------------------------
//| Function   - UI32 power(SI32 base, SI32 exponent)
//| Date       - Unknown
//| Programmer - Abaddon
//| Modified   -
//o--------------------------------------------------------------------------
//| Purpose    - Integer based version of the pow() function
//o--------------------------------------------------------------------------
inline UI32 power(UI32 base, UI32 exponent)
{
    if (exponent == 0)
        return 1;

    if (exponent > 0)
    {
        UI32 total = base;
        for (UI32 i = 1; i < exponent; ++i)
            total *= base;
        return total;
    }

    return 0;
}

}

#endif
