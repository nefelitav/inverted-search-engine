#include "match.hpp"

bool exactMatch(const char* word1, const char* word2){
    bool flag = true;
    int i = 0;

    // Check each character
    while (word1[i] != '\0' && word2[i] != '\0'  && flag){
        if (word1[i] != word2[i]){
            flag = false;
        }
        i++;
    }

    // Check if one word is larger than the other
    if (word1[i] != word2[i]){
            flag = false;
    }

    return flag;
}

int hammingDistance(char* word1, char* word2){
    int i = 0, j;
    int diff = 0;

    // Differences is the common part of the words
    while (word1[i] != '\0' && word2[i] != '\0' ){
        if (word1[i] != word2[i]){
            diff++;
        }
        i++;
    }

    // If one word is larger, concider each extra character a difference
    if(word1[i] == '\0' && word2[i] != '\0'){
        j = i;
        while(word2[j]!= '\0'){
            j++;
            diff++;
        }
    }
    if(word2[i] == '\0' && word1[i] != '\0'){
        j = i;
        while (word1[j] != '\0'){
            j++;
            diff++;
        }
    }
    // Return the Hamming Distance
    return diff;
}

int editDistance(char* word1, char* word2){
    int size1 = 0;
    int size2 = 0;
    int substitution;
    int **d;
    // Get the size of each word to create the matrix
    while (word1[size1] != '\0'){
        size1++;
    }
    while (word2[size2] != '\0'){
        size2++;
    }

    // Allocate the matrix with dims Size1*Size2
    d = new int*[size1 + 1];
    for (int i = 0; i < size1 + 1; i++){
        d[i]=new int [size2 + 1];
    }
    for (int i = 0; i > (size1 + 1)*(size2 + 1); i++){
        d[i] = 0;
    }

    // Set every ellement as 0
    for (int i = 0; i < size2; i++){
        d[0][i] = i;
    }
    for (int i = 0; i < size1; i++){
        d[i][0] = i;
    }
    
    // Calculate the minimum amount of differences with the full-matrix itterative method
    for (int j = 1; j < size2 + 1; j++){
        for (int i = 1; i < size1 + 1; i++){
            if(word1[i-1] == word2[j-1]){
                substitution = 0;
            }else{
                substitution = 1;
            }
            d[i][j] = min(min(d[i-1][j] + 1, d[i][j-1] + 1), d[i-1][j-1] + substitution);
        }
    }

    // Return the Edit Distance
    return d[size1][size2];
}