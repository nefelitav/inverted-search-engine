#include "structs.hpp"
#include "functions.hpp"

int main()
{
    /*
    char *q_words = new char[MAX_QUERY_LENGTH];
    memcpy(q_words, "hello\0world\0how\0are\0you\0", MAX_QUERY_LENGTH);
    Query q(q_words, 1);
    q.printQuery();
    cout << q.getText() << endl;
    cout << q.getWord(3) << endl;
    delete[] q_words;
    */
    const char* s = "hello\0 world\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0 hello\0";
    int i = 0;
    while(s[i] != '\0' || s[i+1] != '\0')
    {
        i++;
    }
    char *d_words = new char[i + 1 + 1]; // 1 -> for the two /0 in the end
    memcpy(d_words, s, i + 1 + 1); 
    Document d(d_words, 1);
    Deduplication(&d);
    d.printDocument();
    cout << d.getText() << endl;
    cout << d.getWord(4) << endl;

    const word w = d.getWord(1);
    const word w2 = d.getWord(2);

    entry* e = NULL;
    entry* e2 = NULL;
    entry_list* el = NULL;
    
    create_entry_list(&el);
    create_entry(&w, &e);
    create_entry(&w2, &e2);

    add_entry(el, e);
    add_entry(el, e2);

    cout << e2->getWord() << endl;
    cout << get_number_entries(el) << endl;
    cout << get_first(el)->getWord() << endl;
    cout << get_next(el, e2)->getWord() << endl;

    //destroy_entry(e);
    //destroy_entry(e2);
    destroy_entry_list(el);
    delete[] d_words;


}