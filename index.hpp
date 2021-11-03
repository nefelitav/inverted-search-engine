#ifndef INDEX
#define INDEX

#include "structs.hpp"
#include "core.h"
#include "treeNodeList.hpp"
#include "childQueue.hpp"
class index{
    private:
        MatchType indexMatchingType;
        class entry* content;
        class treeNodeList* children;
    public:
        index(entry* input, MatchType matchingMetric);
        ~index();
        int addEntry(entry* input);
        int printTree(int depth = 0);
        class entry* getEntry();
        class treeNodeList* getChildren();
        MatchType getMatchingType();
};

ErrorCode buildEntryIndex(const entry_list* el, MatchType type, class index** ix);
ErrorCode destroy_entry_index(class index* ix);
ErrorCode lookup_entry_index(const word* w,class index* ix, int threshold, entry_list** result);
#endif  //INDEX