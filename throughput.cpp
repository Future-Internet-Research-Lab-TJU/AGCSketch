#include<iostream>
#include<stdio.h>
#include<fstream>

#include<vector>
#include<unordered_map>


#include "../AGCSketch/agc.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

#define START_FILE_NO 1
#define END_FILE_NO 1
#define WARM_UP_TIME 20
#define RUN_TIME 30

#define Assault_MEM (450 * 1024)  
#define BUCKET_NUM (Assault_MEM / 8)  
#define TOT_MEM_IN_BYTES (500 * 1024)   

vector<string> traces[END_FILE_NO - START_FILE_NO +1];
unordered_map<string, int> Item[END_FILE_NO - START_FILE_NO +1];
struct five_tuple { char key[13]; };
typedef vector<five_tuple> Trace;


void ReadInTraces(const char *trace_prefix)
{
	cout << "****************basic information of datesets****************" << endl;

	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		//FILE *fin = fopen(datafileName, "rb");
		ifstream fin(datafileName, ios::binary);
		ios::sync_with_stdio(false);
		char key[13];
		traces[datafileCnt - 1].clear();
		
		while(1)
		{
			fin.read(key, KEY_LENGTH);
			if (fin.gcount() < 13) break;
			string str(key, KEY_LENGTH);
			traces[datafileCnt - 1].push_back(str);
			Item[datafileCnt - 1][str]++;
		}
        fin.close();
		printf("Successfully read in %s, %ld packets, %ld flows\n", datafileName, traces[datafileCnt - 1].size(),Item[datafileCnt-1].size());
	}
	printf("\n");
}

//throughput test
AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *agc = NULL;
void insert_query_test(AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> * agc, double& insert_time, double& query_time)
{
	
	//insert test
	steady_clock::time_point t1 = steady_clock::now();  
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; datafileCnt++)
    {
        for (auto key:traces[datafileCnt - 1])
        {
            agc->insert(key.c_str(), KEY_LENGTH);
        }
	}
	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span1 = duration_cast<duration<double>>(t2 - t1);
	insert_time = time_span1.count();

	//query test
	steady_clock::time_point t3 = steady_clock::now();
	for(auto it:Item[0])
		agc->query(it.first.c_str());
	steady_clock::time_point t4 = steady_clock::now();
	duration<double> time_span2 = duration_cast<duration<double>>(t4 - t3);
	query_time = time_span2.count();
}

void throughput_test(FILE *output_file)
{
	fprintf(output_file,"\n***********************throughput test**************************\n");
	AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> *agc = NULL;
	double ave_insert_time = 0,ave_query_time = 0;
	double insert_time,query_time;

	for(int i = 0; i < WARM_UP_TIME + RUN_TIME; i++)
	{
        agc = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
		insert_query_test(agc, insert_time, query_time);
		if(i >= WARM_UP_TIME)
		ave_insert_time += insert_time;
		ave_query_time += query_time;
		delete agc;
	}

	ave_insert_time /= RUN_TIME, ave_query_time /= RUN_TIME;
	double insert_throughput = traces[0].size() / ave_insert_time / 1e6;
	double query_throughput = Item[0].size() / ave_query_time / 1e6;
	fprintf(output_file,"agc\tinsert_throughput:%lf Mpps\t\tquery_throughput:%lf Mpps\n",insert_throughput,query_throughput);
	cout << insert_throughput <<endl;
	cout << query_throughput <<endl;
}

int main()   
{
	ReadInTraces("../../data/"); 
	FILE * output_file = fopen("agc_txt", "a+");

	throughput_test(output_file);
	fclose(output_file);

	return 0;
}

