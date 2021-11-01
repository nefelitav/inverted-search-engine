#include "functions.hpp"
#include "structs.hpp"
#include <cmath>


bool exactMatch(const char* word1, const char* word2) {
    bool flag = true;
    int i = 0;

    // Check each character
    while (word1[i] != '\0' && word2[i] != '\0'  && flag) {
        if (word1[i] != word2[i]) {
            flag = false;
        }
        i++;
    }

    // Check if one word is larger than the other
    if (word1[i] != word2[i]) {
            flag = false;
    }

    return flag;
}

int hammingDistance(const char* word1, const char* word2) {
    int i = 0, j;
    int diff = 0;

    // Differences is the common part of the words
    while (word1[i] != '\0' && word2[i] != '\0' ) {
        if (word1[i] != word2[i]){
            diff++;
        }
        i++;
    }

    // If one word is larger, concider each extra character a difference
    if(word1[i] == '\0' && word2[i] != '\0') {
        j = i;
        while(word2[j]!= '\0'){
            j++;
            diff++;
        }
    }
    if(word2[i] == '\0' && word1[i] != '\0') {
        j = i;
        while (word1[j] != '\0'){
            j++;
            diff++;
        }
    }
    // Return the Hamming Distance
    return diff;
}

int editDistance(const char* word1,const char* word2) {
    int final_distance;
    int size1 = 0;
    int size2 = 0;
    int substitution;
    int **d;
    // Get the size of each word to create the matrix
    while (word1[size1] != '\0') {
        size1++;
    }
    while (word2[size2] != '\0') {
        size2++;
    }

    // Allocate the matrix with dims Size1*Size2
    d = new int*[size1 + 1];
    for (int i = 0; i < size1 + 1; i++) {
        d[i]=new int [size2 + 1];
    }
    for (int i = 0; i > (size1 + 1)*(size2 + 1); i++) {
        d[i] = 0;
    }

    // Initialize border elements
    for (int i = 0; i < size2; i++) {
        d[0][i] = i;
    }
    for (int i = 0; i < size1; i++) { 
        d[i][0] = i;
    }
    
    // Calculate the minimum number of differences with the full-matrix itterative method
    for (int j = 1; j < size2 + 1; j++) {
        for (int i = 1; i < size1 + 1; i++) {
            if(word1[i-1] == word2[j-1]) {
                substitution = 0;
            }else{
                substitution = 1;
            }
            d[i][j] = min(min(d[i-1][j] + 1, d[i][j-1] + 1), d[i-1][j-1] + substitution);
        }
    }
    final_distance=d[size1][size2];

    // Delete the matrix
    for (int i = 0; i < size1 + 1; i++) {
        delete d[i];
    }
    delete d;
   
    // Return the Edit Distance
    return final_distance;
}

/////////////////////////////////////////////////////////////////////

int binarySearch(word* words, int left, int right, const word w)
{
    int mid, cmp;
    while (left < right) 
    {
        mid = left + (right - left) / 2;
        cmp = strcmp(w, words[mid]);
        
        if (cmp == 0)
        {
            return left;//-1;                          // word already in array -> continue
        }
        else if (cmp > 0)                       // word should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0)                       // word should go before mid
        {
            right = mid - 1;
        }
    }
    cmp = strcmp(w, words[left]);
    if (cmp > 0)
    {
        return left + 1;
    }
    else if (cmp < 0)
    {
        return left;
    }
    return left;
    //return -1;
}

HashTable :: HashTable()
{
    this->buckets = new word*[MAX_BUCKETS]();                              // pointers to buckets
    //for (int bucket = 0; bucket < MAX_BUCKETS; bucket++) 
    //{
    //    this->buckets[bucket] = NULL;
    //}
    this->wordsPerBucket = new int[MAX_BUCKETS]();
    cout << "Hash Table is created" << endl;
}

void HashTable :: addToBucket(int hash, word w)
{
    cout << "hash " << hash << endl;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new word[5]();                  // create bucket
        for (int i = 0; i < 5; i++)
        {
            this->buckets[hash][i] = new char[MAX_WORD_LENGTH];
        }
        this->buckets[hash][0] = w;
        this->wordsPerBucket[hash]++;     
        //cout << "Bucket no " << hash << " is created" << endl;
        return;
    }
    if (this->wordsPerBucket[hash] % 5 == 5 || this->wordsPerBucket[hash] == 5)
    {
        word* resized = new char*[MAX_WORD_LENGTH * (5 + this->wordsPerBucket[hash])]();  
        resized = this->buckets[hash];
        if (this->buckets[hash] != NULL)
        {
            delete [] this->buckets[hash];
            this->buckets[hash] = NULL;
        }
        this->buckets[hash] = resized;
    }
    int pos = binarySearch(this->buckets[hash], 0, this->wordsPerBucket[hash] - 1, w);
    //cout << pos << endl;
    
    if (pos == -1)
    {
        return;
    }
    //cout << pos << endl;

    if (this->wordsPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (int i = this->wordsPerBucket[hash]; i > pos; i--)
        {
            strcpy(this->buckets[hash][i], this->buckets[hash][i-1]); 
        }
    }
    strcpy(this->buckets[hash][pos], w);                              // new node in array
    this->wordsPerBucket[hash]++;     
    cout << this->wordsPerBucket[hash] << endl;
    cout << this->buckets[hash][pos] << endl;

    

}
void HashTable :: printTable() 
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        cout << "Bucket no " << bucket << endl;
        for (int word = 0; word < this->wordsPerBucket[bucket]; word++) 
        {
            cout << this->buckets[bucket][word] << endl;
        }
        cout << "######################" << endl;
    }
    
}
HashTable :: ~HashTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->buckets[bucket] != NULL)
        {
            for (int w = 0; w < this->wordsPerBucket[bucket]; w++)
            {
                cout << this->wordsPerBucket[bucket] << endl;
                //if (this->buckets[bucket][w] != NULL)
                delete [] this->buckets[bucket][w];
            }
            delete [] this->buckets[bucket];
        }
        //cout << "Bucket no " << bucket << " is deleted !" << endl;
    }
    delete[] this->wordsPerBucket;
    delete[] this->buckets;
    cout << "Hash Table is deleted" << endl;
}

unsigned long hashFunction(char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; 
    }

    return hash % (int)(ceil (sqrt(MAX_DOC_WORDS / (80.0/100)) ) );
}

void Deduplication(Document* d)
{
    HashTable HT;
    for (int i = 0; i < MAX_DOC_WORDS; i ++)
    {
        char* w = d->getWord(i);
        //cout << w << endl;
        if ( w == NULL)
        {
            break;
        }
        //HT.addToBucket(hashFunction(w), w);
        //HT.printTable();
    }
    
}