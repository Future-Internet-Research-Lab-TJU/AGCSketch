#include<stdio.h>
#include<fstream>
#include<set>
#include<vector>
#include <chrono>

#include "../AGCSketch/agc.h"

using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 2

#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 0.02/100)

#define Guide_Candidate_Mem (50 * 1024)
#define BUCKET_NUM ((TOT_MEM_IN_BYTES - Guide_Candidate_Mem) / 8) 
#define TOT_MEM_IN_BYTES (350 * 1024)   //定义分配的内存大小600KB


vector<string> traces[END_FILE_NO - START_FILE_NO +1];
unordered_map<string, int> Item[END_FILE_NO - START_FILE_NO +1];

void ReadInTraces(const char *trace_prefix)
{
	cout << "****************basic information of datesets****************" << endl;

	for (int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		
		ifstream fin(datafileName, ios::binary);
        if(!fin )
            cout<<"读取错误"<<endl;
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
		//printf("Successfully read in %s, %ld packets, %ld flows\n", datafileName, traces[datafileCnt - 1].size(),Item[datafileCnt-1].size());
        cout << "包数" << traces[datafileCnt - 1].size() << "  " << "流数" << Item[datafileCnt-1].size() << endl;
	}
	printf("\n");
}

void heavy_change_test()
{
    AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> * agc1 = NULL;
    AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES> * agc2 = NULL;
    
    //第一个sketch
	    agc1 = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();      
	    for (auto key:traces[0])  
        {	
            agc1->insert(key.c_str(), 1);          
		}
        
        //第二个sketch
        agc2 = new AGCSketch<BUCKET_NUM, TOT_MEM_IN_BYTES>();
        for (auto key:traces[1])  
        {	
            agc2->insert(key.c_str(), 1);          
		}

		//存储所有流在两个数据集下的真实差值
        unordered_map<string, int> answer;
        unordered_map<string, int> real;

        for (auto it:Item[0]){
            (answer)[it.first] += it.second; //将trace1中的流放入到answer中
        }
        for (auto it:Item[1]){
            (answer)[it.first] -= it.second; //直接把trace2放入到answer中时做出差值记录，最终answer中存储的是所有流的差值
        }
        

        //统计总的差值作为判断的阈值
        int threshold = 0;
        for(unordered_map<string, int>::iterator it = answer.begin(); it != answer.end(); it++){
            threshold += abs(it->second) * 0.05 / 100;
        }

        int tot_real = 0;
        for (unordered_map<string, int>::iterator it = answer.begin(); it != answer.end(); it++){
            if(abs(it->second) >= threshold){
                tot_real++;  //RR的分母，真实的heavy change值
                (real)[it->first] = it->second;
            }
        }

		unordered_map<string, int> heavy_change;
        unordered_map<string, int> est;
        
        for(auto it : Item[0]){
            (heavy_change)[it.first] = agc1->query(it.first.c_str()) - agc2->query(it.first.c_str());  //将查询得到的差值放入到heavy change中
        }

        for(auto it : Item[1]){
            (heavy_change)[it.first] = agc1->query(it.first.c_str()) - agc2->query(it.first.c_str());  //将查询得到的差值放入到heavy change中
        }

        int tot_report = 0;
        for (unordered_map<string, int>::iterator it = heavy_change.begin(); it != heavy_change.end(); it++){
            if(abs(it->second) >= threshold){
                tot_report++;  //RR的分母，真实的heavy change值
                (est)[it->first] = it->second;
            }
        }

		int hit = 0;
        double ARE = 0, avg_PR = 0, avg_RR = 0, avg_ARE = 0, avg_F1 = 0;

        for (unordered_map<string, int>::iterator it = est.begin(); it !=  est.end(); it++){
            //PR的分母，测量的heavy change值
                //遍历it，查找answer中是否存在相同key
            for(unordered_map<string, int>::iterator iter = real.begin(); iter !=  real.end(); iter++){
                if(iter->first == it->first ){
                    hit++;
                }                    
            }            
        }
                

        avg_PR += (double)(hit) / (double)tot_report ;
        avg_RR += (double)(hit) / (double)tot_real ;
        avg_ARE += ARE / (double)hit;
        avg_F1 += 2 * (double)hit / (double)(tot_report + tot_real) ;

        cout << avg_F1 << " " << avg_PR << " " << avg_RR << endl;
        //cout << hit << " " << tot_real << " " << tot_report << endl;
        //cout << heavy_change.size() << " " << answer.size();
        //cout << threshold << endl;
}

int main()
{  
    ReadInTraces("../../data/");
    
    heavy_change_test();
    return 0;
}
