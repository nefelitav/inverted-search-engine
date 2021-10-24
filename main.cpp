#include "structs.hpp"

int main()
{
    /*
    char *words = new char[MAX_QUERY_LENGTH];
    memcpy(words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(words, 1);
    //q.printQuery();
    //cout << q.getText() << endl;
    //cout << q.getWord(5) << endl;
    delete[] words;
    */
    const char* s = "hello\0 world\0 how\0 are\0 you\0";
    int i = 0, count = 0;
    while(s[i] != '\0' || s[i+1] != '\0')
    {
        i++;
        count += 1;
    }
    char *words2 = new char[count + 1 + 1]; // 1 -> for the two /0 in the end
    memcpy(words2, s, count + 1 + 1); 
    Document d(words2, 1);
    //Deduplication(&d);
    //d.printDocument();
    //cout << d.getText() << endl;
    word w = d.getWord(1);
    word w2 = d.getWord(2);
    //entry e(w, 1);

    entry* e = NULL;
    entry* e2 = NULL;
    entry_list* el = NULL;
    
    create_entry_list(&el);
    create_entry(&w, &e);
    create_entry(&w2, &e2);

    add_entry(el, e);
    add_entry(el, e2);

    //cout << e->getWord() << endl;
    //cout << get_number_entries(el) << endl;
    //get_first(el);
    //get_next(el);

    destroy_entry(e);
    destroy_entry(e2);
    destroy_entry_list(el);
    delete[] words2;



}