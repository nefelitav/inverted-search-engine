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

ErrorCode destroy_entry_list(entry_list* el) // first delete entries and then list
{
    try {
        entry * curr = el->getHead();       // pointer to head of list
        entry * next = NULL;

        while ( curr != NULL)               // if null -> end of list
        {
            next = curr->getNext();         // save next entry
            destroy_entry(curr);            // delete entry
            curr = next;                    // go to next entry
        }
        delete el;                          // delete entrylist
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

entry* get_next(entry_list* el, entry* e)
{
    return el->getNext(e);
}
