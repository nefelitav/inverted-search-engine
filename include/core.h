/*
  * core.h version 1.0
  * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
  * Authors: Amin Allam, Fuad Jamour
  *
  * Permission is hereby granted, free of charge, to any person
  * obtaining a copy of this software and associated documentation
  * files (the "Software"), to deal in the Software without
  * restriction, including without limitation the rights to use,
  * copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the
  * Software is furnished to do so, subject to the following
  * conditions:
  *
  * The above copyright notice and this permission notice shall be
  * included in all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  * OTHER DEALINGS IN THE SOFTWARE.
  * 
  * Current version: 1.1 (Feb 13, 2013)
  *
  * Version history:
  *  - 1.1 (Feb 13, 2013)
  *    * Fixed C incompatibility in enum definitions
  *  - 1.0 (Feb 1, 2013)
  *    * Initial release
*/
#ifndef __SIGMOD_CORE_H_
#define __SIGMOD_CORE_H_
#ifdef __cplusplus
#include <cmath>
extern "C" {
    #endif
    
    #define MAX_DOC_LENGTH (1<<22) // (MAX_WORD_LENGTH + 1 + 1) * MAX_DOC_WORDS  
    #define MAX_WORD_LENGTH 31
    #define MIN_WORD_LENGTH 4
    #define MAX_QUERY_WORDS 5
    #define MAX_QUERY_LENGTH ((MAX_WORD_LENGTH+1)*MAX_QUERY_WORDS)
    #define MAX_DOC_WORDS (MAX_DOC_LENGTH /(MIN_WORD_LENGTH + 1))

    // For larger datasets use more and bigger buckets
    #define MAX_BUCKETS 6250 
    #define MAX_QUERY_BUCKETS 2500 
    #define WORDS_PER_BUCKET 10 
    #define QUERIES_PER_BUCKET 400 
    #define ENTRIES_PER_BUCKET 400

    /*#define MAX_BUCKETS  5000
    #define MAX_QUERY_BUCKETS  100 
    #define WORDS_PER_BUCKET 10
    #define QUERIES_PER_BUCKET 20
    #define ENTRIES_PER_BUCKET  10*/
    
    // 0 -> sequential, 
    // 1 -> StartQuery multithreading, 
    // 2 -> MatchDocument multithreading,
    // 3 -> EndQuery multithreading,
    // 4 -> StartQuery & MatchDocument multithreading, 
    // 5 -> EndQuery & MatchDocument multithreading, 
    // 6 -> StartQuery & EndQuery multithreading, 
    // 7 -> everything

    #define PARALLEL 2

    typedef unsigned int QueryID;
    typedef unsigned int DocID;
    

    typedef enum {
        MT_EXACT_MATCH,
        MT_HAMMING_DIST,
        MT_EDIT_DIST
    }
    MatchType;
    
    typedef enum {
        EC_SUCCESS,
        EC_NO_AVAIL_RES,
        EC_FAIL
    }
    ErrorCode;

    ErrorCode InitializeIndex();
    ErrorCode DestroyIndex();
    ErrorCode StartQuery(QueryID        query_id,   
                        const char*    query_str,
                        MatchType      match_type,
                        unsigned int   match_dist);
    ErrorCode EndQuery(QueryID query_id);
    ErrorCode MatchDocument(DocID         doc_id,
                            const char*   doc_str);
    ErrorCode GetNextAvailRes(DocID*         p_doc_id,
                            unsigned int*  p_num_res,
                            QueryID**      p_query_ids);



    #ifdef __cplusplus
}
#endif
 
#endif // __SIGMOD_CORE_H_