#include "functions.hpp"

int exactMatch(const char* word1, const char* word2) {
    if (strcmp(word1, word2) == 0) {
        return 0;
    } else {
        return MAX_WORD_LENGTH + 1;
    }
}

int hammingDistance(const char* word1, const char* word2) {
    int i = 0;

    // if equal, just stop
    if (strcmp(word1, word2) == 0) {
        return 0;
    }

    // If one word is larger, consider each extra character a difference
    int diff = abs((int)strlen(word1) - (int)strlen(word2));

    // Difference in the common part of the words
    while (word1[i] != '\0' && word2[i] != '\0' ) {
        if (word1[i] != word2[i]) {
            diff++;
        }
        i++;
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

    // if equal, just stop
    if (strcmp(word1, word2) == 0) {
        return 0;
    }
    
    // Get the size of each word to create the matrix
    size1 = strlen(word1);
    size2 = strlen(word2);

    // Allocate the matrix with dims Size1*Size2
    d = new int*[size1 + 1];
    for (int i = 0; i < size1 + 1; i++) {
        d[i] = new int [size2 + 1]();
    }

    // Initialize border elements
    for (int i = 0; i < size2; i++) {
        d[0][i] = i;
    }
    for (int i = 0; i < size1; i++) { 
        d[i][0] = i;
    }
    
    // Calculate the minimum number of differences with the full-matrix iterative method
    for (int j = 1; j < size2 + 1; j++) {
        for (int i = 1; i < size1 + 1; i++) {
            if (word1[i-1] == word2[j-1]) {
                substitution = 0;
            } else {
                substitution = 1;
            }
            d[i][j] = std::min(std::min(d[i-1][j] + 1, d[i][j-1] + 1), d[i-1][j-1] + substitution);
        }
    }
    final_distance = d[size1][size2];

    // Delete the matrix
    for (int i = 0; i < size1 + 1; i++) {
        delete[] d[i];
    }
    delete[] d;
   
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
            return -1;                                                      // word already in array -> continue
        }
        else if (cmp > 0)                                                   // word should go after mid
        {
            left = mid + 1;
        }
        else if (cmp < 0)                                                   // word should go before mid
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
    return -1;                                                              // word already in array -> continue
}

HashTable :: HashTable()
{
    this->buckets = new word*[MAX_BUCKETS]();                               // pointers to buckets
    this->wordsPerBucket = new int[MAX_BUCKETS]();                         
    std::cout << "Hash Table is created!" << std::endl;
}

void HashTable :: addToBucket(int hash, const word w)
{
    //cout << "hash = " << hash << endl;
    int i;
    if (this->buckets[hash] == NULL)
    {
        this->buckets[hash] = new char*[100]();                            // create bucket
        for (i = 0; i < 100; i++)
        {
            this->buckets[hash][i] = new char[MAX_WORD_LENGTH];
        }
        strcpy(this->buckets[hash][0], w);                                 // first word in bucket
        this->wordsPerBucket[hash]++;    
        //cout << "Bucket no " << hash << " is created" << endl;
        return;
    }
    if (this->wordsPerBucket[hash] % 100 == 0)                             // have reached limit of bucket
    {
        word* resized = new char*[this->wordsPerBucket[hash] + 100]();     // create bigger bucket
        for (i = 0; i <  this->wordsPerBucket[hash] + 100; i++)
        {
            resized[i] = new char[MAX_WORD_LENGTH];
            if (i < this->wordsPerBucket[hash])
            {
                strcpy(resized[i], this->buckets[hash][i]);                // copy words accumulated until now
            }
        }

        if (this->buckets[hash] != NULL)                                   // delete old bucket
        {
            int wordsNum = 100 + (this->wordsPerBucket[hash] / 100) * 100;
            if (this->wordsPerBucket[hash] % 100 == 0)
            {
                wordsNum = (this->wordsPerBucket[hash] / 100) * 100;
            }
            for (i = 0; i < wordsNum; i++)
            {
                delete [] this->buckets[hash][i];
            }
            delete [] this->buckets[hash];
        }

        this->buckets[hash] = resized;                                    // pointer to the new bigger bucket 
    }

    int pos = binarySearch(this->buckets[hash], 0, this->wordsPerBucket[hash] - 1, w);
    
    if (pos == -1)
    {
        return;
    }
    //cout << pos << endl;

    if (this->wordsPerBucket[hash] >= pos)                             // in the middle of the array
    {
        for (i = this->wordsPerBucket[hash]; i > pos; i--)
        {
            strcpy(this->buckets[hash][i], this->buckets[hash][i-1]); 
        }
    }
    strcpy(this->buckets[hash][pos], w);                              // new node in array
    this->wordsPerBucket[hash]++;  
}
void HashTable :: printTable() 
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->wordsPerBucket[bucket] > 0)
        {
            std::cout << "Bucket no " << bucket << std::endl;
            for (int word = 0; word < this->wordsPerBucket[bucket]; word++) 
            {
                std::cout << this->buckets[bucket][word] << std::endl;
            }
            std::cout << "######################" << std::endl;
        }
    }
}
HashTable :: ~HashTable()
{
    for (int bucket = 0; bucket < MAX_BUCKETS; bucket++)
    {
        if (this->buckets[bucket] != NULL)
        {
            int wordsNum = 100 + (this->wordsPerBucket[bucket] / 100) * 100; // figure out how many words should be deleted
            if (this->wordsPerBucket[bucket] % 100 == 0)
            {
                wordsNum = (this->wordsPerBucket[bucket] / 100) * 100;
            }
            for (int w = 0; w < wordsNum; w++)
            {
                delete [] this->buckets[bucket][w];                         // delete words
            }
            delete [] this->buckets[bucket];                                // delete buckets
        }
        //cout << "Bucket no " << bucket << " is deleted !" << endl;
    }
    delete[] this->wordsPerBucket;
    delete[] this->buckets;                                                 // delete hash table
    std::cout << "Hash Table is deleted!" << std::endl;
}

unsigned long hashFunction(word str)
{
    unsigned long hash = 5381;
    int character;

    while ((character = *str++))
    {
        hash = ((hash << 5) + hash) + character; 
    }

    return hash % MAX_BUCKETS;
}

void Deduplication(Document* d)
{
    HashTable HT;
    for (int i = 0; i < MAX_DOC_WORDS; i ++)
    {
        const word w = d->getWord(i);
        //cout << w << endl;
        if ( w == NULL)
        {
            break;
        }
        HT.addToBucket(hashFunction(w), w);
    }
    HT.printTable();
}