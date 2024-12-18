#include<stdio.h>
#include<fstream>
#include<set>
#include<vector>
#include <chrono>

#include "agc.h"

using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 1

#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 0.02/100)

#define Guide_Candidate_Mem (50 * 1024)
#define BUCKET_NUM ((TOT_MEM_IN_BYTES - Guide_Candidate_Mem) / 8) 
#define TOT_MEM_IN_BYTES (1300 * 1024)   


vector<string> traces[END_FILE_NO - START_FILE_NO +1];
unordered_map<string, int> Item[END_FILE_NO - START_FILE_NO +1];

void ReadInTraces(const char *trace_prefix)
{
	cout << "****************basic information of datesets****************" << endl;

	for (int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		// char datafileName[100];
		// sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		
		// ifstream fin(datafileName, ios::binary);
		// ios::sync_with_stdio(false);
		// char key[13];
		// traces[datafileCnt - 1].clear();
		
		// while(1)
		// {
		// 	fin.read(key, KEY_LENGTH);
		// 	if (fin.gcount() < 13) break;
		// 	string str(key, KEY_LENGTH);
		// 	traces[datafileCnt - 1].push_back(str);
		// 	Item[datafileCnt - 1][str]++;
		// }
        // fin.close(); 
		// printf("Successfully read in %s, %ld packets, %ld flows\n", datafileName, traces[datafileCnt - 1].size(),Item[datafileCnt-1].size());
        const char* dataFileName = "C:/Users/Ocean/Desktop/DataSet/dataset_1.bit";
        ifstream fin(dataFileName, ios::binary);

        traces[datafileCnt - 1].clear();

        while(traces[datafileCnt - 1].size() < 500000){
            Header header;
            fin.read(reinterpret_cast<char*>(&header), sizeof(header));
            if(header.version == 4){
                Packet packet = { header.source_ipv4, header.dest_ipv4, header.source_port, header.dest_port, header.protocol };
                string str = string(reinterpret_cast<char*>(&packet), sizeof(packet));
                traces[datafileCnt - 1].push_back(str); 
		        Item[datafileCnt - 1][str]++;
            }
        }

        fin.close();
        int max_fre = INT16_MIN;
        for(auto it : Item[0]){
            if(it.second > max_fre){
                max_fre = it.second;
            }
        }
        cout << "Max flow size = " << max_fre << endl;
		printf("Successfully read in %s, %ld packets, %ld flows\n", dataFileName, traces[datafileCnt - 1].size(),Item[datafileCnt-1].size());
	}
	printf("\n");
}


//heavy hitter test
void heavy_hitter_test()   //主函数开始
{

	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)  
	{
		
		AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>* agc = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();   
        unordered_map<string, int> Real_Freq; 
		int packet_cnt = (int)traces[datafileCnt - 1].size();  
		for (auto key:traces[datafileCnt - 1]) 
        {	
            agc->insert(key.c_str(), 1);          
            Real_Freq[key]++;
		} 

		set<string> HH_true;
		for(auto it : Real_Freq)
        { 
            if (it.second > HEAVY_HITTER_THRESHOLD(packet_cnt))
            {
                HH_true.insert(it.first);
				// cout << agc->query(it.first.c_str()) << ", " << it.second << endl;
            }
        }

		cout << "true eflow count = " << HH_true.size() << endl;

		vector< pair<std::string, int> > heavy_hitters;
		agc->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters); 


        set<string> HH_estimate;

        int a = 0;
        for (int i = 0; i < heavy_hitters.size(); ++i)
        {
            a ++;
            HH_estimate.insert(heavy_hitters[i].first);
			// cout << heavy_hitters[i].second << ", " << agc->query(heavy_hitters[i].first.c_str()) << endl;
        }

		cout << "record eflow count = " << HH_estimate.size() << endl;

		double avg_F1 = 0,cnt = 0, tp = 0,pre = 0, recall = 0;
        double ARE = 0;
        int est_freq = 0;
        int HH_PR = 0;
        int HH_PR_denom = HH_estimate.size();
        int HH_RR = 0;
        int HH_RR_denom = HH_true.size();
        for (auto it : HH_true)
        {
            HH_RR += HH_estimate.find(it) != HH_estimate.end();
            // cout << agc->query(it.c_str()) << ", " << (HH_estimate.find(it) == HH_estimate.end()) << endl;
            //int est_freq = agc->query(it.c_str());          
            //ARE += fabs(est_freq - Real_Freq[it.c_str()]) / (double)Real_Freq[it.c_str()];
        }
        for (auto it : HH_estimate)
        {
            HH_PR += HH_true.find(it) != HH_true.end();   
			// cout << agc->query(it.c_str()) << ", " << (HH_true.find(it) == HH_true.end()) << endl;
        }
		cout << "RR =  " << HH_RR << ", PR = " << HH_PR << endl;
        for (auto it:Item[datafileCnt - 1]){ 
            if (it.second > HEAVY_HITTER_THRESHOLD(packet_cnt)){
               cnt ++; //620
               int est_freq = agc->query(it.first.c_str());
                if (est_freq > HEAVY_HITTER_THRESHOLD(packet_cnt)){
                    tp ++; //553
                    ARE += fabs(est_freq - it.second) / (double)it.second;
                }
            }       
        }
        avg_F1 = (2 * (double(HH_PR) / double(HH_PR_denom)) * (double(HH_RR) / double(HH_RR_denom))) / ((double(HH_PR) / double(HH_PR_denom)) + (double(HH_RR) / double(HH_RR_denom)));
		pre = double(HH_PR) / double(HH_PR_denom);
		recall = double(HH_RR) / double(HH_RR_denom);
		cout << avg_F1 << endl;
		cout << pre << endl;
		cout << recall << endl;
        
        delete agc;
		
	}

    
}

void heavy_hitter_test1()   
{

	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)  
	{
		
		AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>* agc = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();   
        unordered_map<string, int> Real_Freq; 
		int packet_cnt = (int)traces[datafileCnt - 1].size();  
		for (auto key:traces[datafileCnt - 1])  
        {	
            agc->insert(key.c_str(), 1);          
            Real_Freq[key]++;
		} 

		vector< pair<std::string, int> > heavy_hitters;
		agc->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters); 

		for(int i = 0, j = 0; i < (int)heavy_hitters.size(); ++i)  
		{
			uint32_t srcIP;
			memcpy(&srcIP, heavy_hitters[i].first.c_str(), 4);
			printf("<%.8x, %d>", srcIP, heavy_hitters[i].second);
			if(++j % 5 == 0)
				printf("\n");
			else printf("\t");
		}
		printf("\n");  //

        
        delete agc;
		
	}

    
}


int main()
{  
    ReadInTraces("../../data/");

    heavy_hitter_test();
    //fclose(output_file); 

    return 0;
}

