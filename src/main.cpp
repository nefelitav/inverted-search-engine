#include "../include/structs.hpp"
#include "../include/utilities.hpp"
#include "../include/index.hpp"
#include "../include/core.h"
#include <fstream>
#include <string>
#include <iostream>


/*
 * test_through.cpp version 1.0
 * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
 * Author: Amin Allam
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
 */


#define TEST_MACHINE

#define TEST_SOL
#ifdef TEST_SOL

#ifdef TEST_MACHINE
#include "../include/core.h"
#else
#include "core.h"
#endif

#include <cstdlib>
#include <cstdio>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/time.h>
int GetClockTimeInMilliSec()
{
	struct timeval t2; gettimeofday(&t2,NULL);
	return t2.tv_sec*1000+t2.tv_usec/1000;
}
void PrintTime(int milli_sec, FILE* out_file)
{
	int v=milli_sec;
	int hours=v/(1000*60*60); v%=(1000*60*60);
	int minutes=v/(1000*60); v%=(1000*60);
	int seconds=v/1000; v%=1000;
	int milli_seconds=v;
	int first=1;
	fprintf(out_file, "%d milli-seconds [", milli_sec);
	if(hours) {if(!first) fprintf(out_file, ":"); fprintf(out_file, "%dh", hours); first=0;}
	if(minutes) {if(!first) fprintf(out_file, ":"); fprintf(out_file, "%dm", minutes); first=0;}
	if(seconds) {if(!first) fprintf(out_file, ":"); fprintf(out_file, "%ds", seconds); first=0;}
	if(milli_seconds) {if(!first) fprintf(out_file, ":"); fprintf(out_file, "%dms", milli_seconds); first=0;}
	fprintf(out_file, "]");
}

///////////////////////////////////////////////////////////////////////////////////////////////

char temp[MAX_DOC_LENGTH];

void TestSigmod(const char* test_file_str, int time_limit_seconds, FILE* out_file)
{
	int i, j;
	fprintf(out_file, "Start Test ...\n"); fflush(out_file);
	FILE* test_file=fopen(test_file_str, "rt");

	if(!test_file)
	{
		fprintf(out_file, "Cannot Open File %s\n", test_file_str);
		fflush(out_file);
		return;
	}

	int v=GetClockTimeInMilliSec();
	InitializeIndex();

	unsigned int first_result=0;
	int num_cur_results=0;

	const int max_results=100;

	bool cur_results_ret[max_results];
	unsigned int cur_results_size[max_results];
	unsigned int* cur_results[max_results];

	unsigned int num_processed_docs=0;
	bool file_finished=false;

	while(1)
	{
		char ch;
		unsigned int id;

		// fixed bug fof last batch
		int fres=fscanf(test_file, "%c %u ", &ch, &id);

		if(num_cur_results && (EOF==fres || ch=='s' || ch=='e'))
		{
			for(i=0;i<num_cur_results;i++)
			{
				unsigned int doc_id=0;
				unsigned int num_res=0;
				unsigned int* query_ids=0;

				ErrorCode err=GetNextAvailRes(&doc_id, &num_res, &query_ids);

				if(err==EC_NO_AVAIL_RES)
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned EC_NO_AVAIL_RES, but there is still undelivered documents.\n");
					fflush(out_file);
					return;
				}
				else if(err==EC_FAIL)
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned EC_FAIL.\n");
					fflush(out_file);
					return;
				}
				else if(err!=EC_SUCCESS)
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned unknown error code.\n");
					fflush(out_file);
					return;
				}

				if(doc_id<first_result || doc_id-first_result>=(unsigned int)num_cur_results)
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned unknown document ID %u.\n", doc_id);
					fflush(out_file);
					return;
				}
				if(cur_results_ret[doc_id-first_result])
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned document (ID=%u) that has been delivered before.\n", doc_id);
					fflush(out_file);
					return;
				}

				bool flag_error=false;

				if(num_res!=cur_results_size[doc_id-first_result])
				{
					flag_error=true;
				}

				for(j=0;j<(int)num_res && !flag_error;j++)
				{
					if(query_ids[j]!=cur_results[doc_id-first_result][j])
					{
						flag_error=true;
					}
				}

				if(flag_error)
				{
					fprintf(out_file, "The call to GetNextAvailRes() returned incorrect result for document ID %u.\n", doc_id);
					fprintf(out_file, "Your  answer   is: "); for(j=0;j<(int)num_res;j++) {if(j)fprintf(out_file, " "); fprintf(out_file, "%u", query_ids[j]);} fprintf(out_file, "\n");
					fprintf(out_file, "Correct answer is: "); for(j=0;j<(int)cur_results_size[doc_id-first_result];j++) {if(j)fprintf(out_file, " "); fprintf(out_file, "%u", cur_results[doc_id-first_result][j]);} fprintf(out_file, "\n");
					fflush(out_file);
					return;
				}

				cur_results_ret[doc_id-first_result]=true;
				if(num_res && query_ids) free(query_ids);

				num_processed_docs++;
				//if(num_processed_docs%1000==0) {fprintf(out_file, "Your program has processed %u documents.\n", num_processed_docs); fflush(out_file);}
			}

			for(i=0;i<num_cur_results;i++) {free(cur_results[i]); cur_results[i]=0; cur_results_size[i]=0; cur_results_ret[i]=false;}
			num_cur_results=0;

			int elapsed_time=GetClockTimeInMilliSec()-v;
			if(time_limit_seconds && elapsed_time>time_limit_seconds*1000)
			{
				if(EOF==fres) file_finished=true;
				break;
			}
		}

		if(EOF==fres)
		{
			file_finished=true;
			break;
		}

		if(ch=='s')
		{
			int match_type;
			int match_dist;

			if(EOF==fscanf(test_file, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp))
			{
				fprintf(out_file, "Corrupted Test File.\n");
				fflush(out_file);
				return;
			}
			
			ErrorCode err=StartQuery(id, temp, (MatchType)match_type, match_dist);

			if(err==EC_FAIL)
			{
				fprintf(out_file, "The call to StartQuery() returned EC_FAIL.\n");
				fflush(out_file);
				return;
			}
			else if(err!=EC_SUCCESS)
			{
				fprintf(out_file, "The call to StartQuery() returned unknown error code.\n");
				fflush(out_file);
				return;
			}
		}
		else if(ch=='e')
		{
			ErrorCode err=EndQuery(id);

			if(err==EC_FAIL)
			{
				fprintf(out_file, "The call to EndQuery() returned EC_FAIL.\n");
				fflush(out_file);
				return;
			}
			else if(err!=EC_SUCCESS)
			{
				fprintf(out_file, "The call to EndQuery() returned unknown error code.\n");
				fflush(out_file);
				return;
			}
		}
		else if(ch=='m')
		{
			if(EOF==fscanf(test_file, "%*u %[^\n\r] ", temp))
			{
				fprintf(out_file, "Corrupted Test File.\n");
				fflush(out_file);
				return;
			}

			ErrorCode err=MatchDocument(id, temp);

			if(err==EC_FAIL)
			{
				fprintf(out_file, "The call to MatchDocument() returned EC_FAIL.\n");
				fflush(out_file);
				return;
			}
			else if(err!=EC_SUCCESS)
			{
				fprintf(out_file, "The call to MatchDocument() returned unknown error code.\n");
				fflush(out_file);
				return;
			}
		}
		else if(ch=='r')
		{
			unsigned int num_res=0;
			if(EOF==fscanf(test_file, "%u ", &num_res))
			{
				fprintf(out_file, "Corrupted Test File.\n");
				fflush(out_file);
				return;
			}
			
			if(num_cur_results==0) first_result=id;
			unsigned int qid;
			cur_results_ret[num_cur_results]=false;
			cur_results_size[num_cur_results]=num_res;
			cur_results[num_cur_results]=(unsigned int*)malloc(num_res*sizeof(unsigned int));
			
			for(i=0;i<(int)num_res;i++)
			{
				if(EOF==fscanf(test_file, "%u ", &qid))
				{
					fprintf(out_file, "Corrupted Test File.\n");
					fflush(out_file);
					return;
				}
				
				cur_results[num_cur_results][i]=qid;
			}
			num_cur_results++;
		}
		else
		{
			fprintf(out_file, "Corrupted Test File. Unknown Command %c.\n", ch);
			fflush(out_file);
			return;
		}
	}

	v=GetClockTimeInMilliSec()-v;

	DestroyIndex();

	fclose(test_file);

	double throughput=(double)num_processed_docs*1000.0/v;

	fprintf(out_file, "Your program has successfully passed all tests.\n");
	fprintf(out_file, "Processed Documents = %u documents. ", num_processed_docs);
	if(file_finished) fprintf(out_file, " (all documents are processed)\n");
	else fprintf(out_file, " (not all documents are processed)\n");
	fprintf(out_file, "Time = "); PrintTime(v, out_file); fprintf(out_file, "\n");
	fprintf(out_file, "Throughput = %0.3lf documents/second.\n", throughput);

	fflush(out_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
#ifdef TEST_MACHINE
	FILE* out_file=fopen("result.txt", "wt");
	if(argc<=1) TestSigmod("./test_data/small_test.txt", 60, out_file);
#else
	FILE* out_file=stdout;
	if(argc<=1) TestSigmod("small_test.txt", 0, out_file);
	//if(argc<=1) TestSigmod("large_test.txt", 60, out_file);
	//if(argc<=1) TestSigmod("huge_test.txt", 120, out_file);
#endif
	else
	{
		int i, time_limit_seconds=0;
		if(argc>=3)
		{
			char* se=argv[2];
			for(i=0;se[i];i++)
			{
				if(se[i]<'0' || se[i]>'9') {time_limit_seconds=-1; break;}
				time_limit_seconds=time_limit_seconds*10+(se[i]-'0');
			}
			if(time_limit_seconds<0)
			{
				fprintf(out_file, "Invalid Time Limit (%s). Please specify time limit as integral number of seconds > 0.\n", se);
				fflush(out_file);
				return 0;
			}
		}
		TestSigmod(argv[1], time_limit_seconds, out_file);
	}
#ifdef TEST_MACHINE
	fclose(out_file);
#else
	fflush(NULL);
#endif
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#endif










// // create documents and queries, reading words from a file
// int main()
// {
//     unsigned int numRez;
//     DocID rezID;
//     QueryID *matchedQueries;

//     std::string line;
//     std::ifstream myfile;
//     myfile.open("./data/input.txt");
//     char *q_words;
//     char *d_words;
//     int q_count = 0, d_count = 0, doc = 0;

//     InitializeIndex();
//     if (myfile.is_open())
//     {
//         while (std::getline(myfile, line)) // read each line
//         {
//             if (line.empty()) // no more queries
//             {
//                 if (doc == 1) // end of file
//                 {
//                     break;
//                 }
//                 doc = 1; // reading documents from now on
//                 continue;
//             }
//             if (doc == 0)
//             {
//                 q_words = new char[strlen(line.c_str()) + 1]();
//                 strcpy(q_words, line.c_str());
//                 StartQuery(q_count, q_words, MT_EXACT_MATCH, 0);
//                 delete[] q_words;
//                 q_count++;
//             }
//             else
//             {
//                 //editIndex->printTree();
//                 d_words = new char[strlen(line.c_str()) + 1]();
//                 strcpy(d_words, line.c_str());
//                 //ET->printTable();
//                 MatchDocument(d_count, d_words);
//                 delete[] d_words;
//                 d_count++;
//             }
//         }
//         myfile.close();
//     }
//     else
//     {
//         std::cout << "Unable to open file" << std::endl;
//         return -1;
//     }

//     while (GetNextAvailRes(&rezID, &numRez, &matchedQueries) != EC_NO_AVAIL_RES)
//     {
//         std::cout << "Document: " << rezID << " matched with: \n";
//         for (unsigned int i = 0; i < numRez; i++)
//         {
//             std::cout << "Query: " << matchedQueries[i] << "\n";
//         }
//     }
//     // EndQuery(0);
//     // EndQuery(1);
//     // EndQuery(2);
//     // EndQuery(3);
//     // EndQuery(4);
//     //ET->printTable();

//     // ET->printTable();
//     // QT->printTable();

//     // const word w = d[0]->getWord(1);
//     // const word w2 = d[0]->getWord(2);
//     // entry *e = NULL;
//     // entry *e2 = NULL;
//     // entry_list *el = NULL;
//     // entry_list *result = NULL;
//     // create_entry_list(&el);
//     // create_entry(&w, &e);
//     // create_entry(&w2, &e2);
//     // add_entry(el, e);
//     // add_entry(el, e2);
//     // std::cout << e->getWord() << std::endl;
//     // std::cout << e2->getWord() << std::endl;
//     // std::cout << get_number_entries(el) << std::endl;
//     // std::cout << get_first(el)->getWord() << std::endl;
//     // std::cout << get_next(el, e2)->getWord() << std::endl;

//     // addToIndex(&e, 1, MT_EDIT_DIST, 1);
//     // addToIndex(&e2, 1, MT_EDIT_DIST, 1);
//     // // Create the list that will store the search results
//     // create_entry_list(&result);

//     // // The search term
//     // const word search_term = (char *)"ipsum";

//     // //Search the tree
//     // lookup_entry_index(&search_term, result, MT_EDIT_DIST);

//     // // Present the result(s)
//     // entry *result_node = result->getHead();
//     // std::cout << "Search Results for " << search_term << ":\n";
//     // while (result_node)
//     // {
//     //     std::cout << result_node->getWord() << "\n";
//     //     result_node = result_node->getNext();
//     // }

//     // // Destroy_entries and entrylist
//     // destroy_entry_list(el);
//     // destroy_entry_list(result);

//     DestroyIndex();
// }