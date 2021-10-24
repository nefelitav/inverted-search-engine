#include "structs.hpp"

ErrorCode create_entry(const word* w, entry** e)
{
    try {
        *e = new entry(*w);
        //cout << (**e).getWord() << endl;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry(entry *e)
{
    try {
        delete e;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode create_entry_list(entry_list** el)
{
    try {
        *el = new entry_list();
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

ErrorCode destroy_entry_list(entry_list* el)
{
    try {
        delete el;
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

unsigned int get_number_entries(entry_list* el)
{
    return el->getEntryNum();
}

ErrorCode add_entry(entry_list* el, entry* e)
{
    try {
        el->addEntry(e);
        return EC_SUCCESS;
    } catch (const exception& _) {
        return EC_FAIL;
    }
    return EC_NO_AVAIL_RES;
}

entry* get_first(entry_list* el)
{
    return el->getHead();
}

entry* get_next(entry_list* el)
{
    return el->getHead()->getNext();
}
