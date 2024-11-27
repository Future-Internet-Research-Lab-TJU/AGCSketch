#include<iostream>
#include<stdio.h>
#include<fstream>

#include<vector>
#include<unordered_map>

#include "agc.h"
using namespace std;
//using namespace std::chrono;

#define START_FILE_NO 1
#define END_FILE_NO 1


#define Guide_Candidate_Mem (50 * 1024)
#define BUCKET_NUM ((TOT_MEM_IN_BYTES - Guide_Candidate_Mem) / 12)  
#define TOT_MEM_IN_BYTES (650 * 1024)  


struct FIVE_TUPLE{	char key[13];	};
vector<string> traces[END_FILE_NO - START_FILE_NO +1];
unordered_map<string, int> Item[END_FILE_NO - START_FILE_NO +1];


//read in traces
void ReadInTraces(const char *trace_prefix)
{
	cout << "****************basic information of datesets****************" << endl;

	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		ifstream fin(datafileName, ios::binary);
        if(!fin )
            cout<<"读取错误"<<endl;
		ios::sync_with_stdio(false);
		char key[8];
		traces[datafileCnt - 1].clear();
		
		while(1)
		{
			fin.read(key, 13);
			if (fin.gcount() < 13) break;
			string str(key, KEY_LENGTH);
			traces[datafileCnt - 1].push_back(str); 
			Item[datafileCnt - 1][str]++; 
		}

        // const char* dataFileName = "C:/Users/Ocean/Desktop/DataSet/dataset_1.bit";
        // ifstream fin(dataFileName, ios::binary);

        // traces[datafileCnt - 1].clear();

        // while(traces[datafileCnt - 1].size() < 500000){
        //     Header header;
        //     fin.read(reinterpret_cast<char*>(&header), sizeof(header));
        //     if(header.version == 4){
        //         Packet packet = { header.source_ipv4, header.dest_ipv4, header.source_port, header.dest_port, header.protocol };
        //         string str = string(reinterpret_cast<char*>(&packet), sizeof(packet));
        //         traces[datafileCnt - 1].push_back(str); 
		//         Item[datafileCnt - 1][str]++;
        //     }
        // }

        fin.close();
        int max_fre = INT16_MIN;
        for(auto it : Item[0]){
            if(it.second > max_fre){
                max_fre = it.second;
            }
        }
        cout << max_fre << endl;
		// printf("Successfully read in %s, %ld packets, %ld flows\n", dataFileName, traces[datafileCnt - 1].size(),Item[datafileCnt-1].size());
	}
	printf("\n");
}


//frequecy test
void frequency_test(int memory,  FILE* output_file)
{
    char datafileName[100];
    fprintf(output_file, "************accuracy test of flow siz with %s************\n",datafileName);

    unordered_map<string, int> Real_Freq;
    for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; datafileCnt++)
    {
        AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>* agc = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
		for (auto key:traces[datafileCnt - 1])  
		{
			agc->insert(key.c_str(), 1); 
        }  


        double ARE = 0;
        double AAE = 0;
        int a = 0;
        for (auto it:Item[datafileCnt - 1])  
        {
            int  est_val = agc->query(it.first.c_str());
            ARE += std::abs(est_val - it.second) * 1.0 / (it.second);
            AAE += std::abs(est_val - it.second);          
        }
        

        ARE /= Item[datafileCnt - 1].size();
        AAE /= Item[datafileCnt - 1].size();

        delete(agc);

       cout << ARE << endl;
       cout << AAE << endl;

    }
    
}


int main()
{  
    ReadInTraces("../../data/");
    
    FILE* output_file = fopen("agc_result.txt", "a+");

    frequency_test(TOT_MEM_IN_BYTES, output_file);


    fclose(output_file); 

    return 0;
}