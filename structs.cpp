#include "structs.hpp"

ErrorCode create_entry(const word* w, entry** e)
{
    try {
        *e = new entry(*w, 0);
        //cout << (**e).getWord() << endl;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry(entry **e)
{
    try {
        delete *e;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}