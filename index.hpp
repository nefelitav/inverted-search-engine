#ifndef INDEX
#define INDEX
#include "structs.hpp"
#include "core.h"
#include "treeNodeList.hpp"


class index{
    private:
        MatchType indexMatchingType;
        class entry* content;
        class treeNodeList* children;
    public:
        index(entry* input, MatchType matchingMetric);
        ~index();
        int addEntry(entry* input);
        int printTree();
        ErrorCode buildEntryIndex(const entry_list* el, MatchType type, index* ix);
        ErrorCode destroy_entry_index(index* ix);
        int search(char* word,int threshold);
        MatchType getMatchingType();
};
#endif  //INDEX