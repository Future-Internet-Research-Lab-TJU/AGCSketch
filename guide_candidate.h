#ifndef _guide_candidate_H
#define _guide_candidate_H


#include<stdlib.h>
#include "BOBHash32.h"
#include "param.h"
using namespace std;

template<int guide_candidate_mem>

class Guide_Candidate_Part 
{  	
private:

    uint32_t shift0,buf0,index;
    uint32_t *counters[m];  //4个指针数组，每个指针指向一个行数组
	uint32_t idx[d];
	uint32_t w[m];  //每一行的计数器数组，即4个数组
	uint32_t min_value = UINT32_MAX;
    BOBHash32 * hash[d];  
public:
    uint32_t value = 0;
	
	Guide_Candidate_Part() { init();}
	~Guide_Candidate_Part() { clear(); }  

    void init(){  //初始化一维数组
		uint32_t in_w = guide_candidate_mem / 4 / 4;  //换算成字节后算出每行32比特的数量
		for(int i =0; i < m; i++)
		{
			counters[i]= new uint32_t[in_w];  //将指针数组指向了行数组的首地址，并分配in_w个大小的内存空间,并声明每个数组的计数器类型是uint32_t
			if(i < 2)
				w[i]= in_w <<4;
			else 
				w[i]= in_w <<(5-i);
			memset(counters[i], 0, in_w * sizeof(uint32_t));		
		}	
		for(int i=0; i<d; i++)
		{
			hash[i] = new BOBHash32(i + 750);
		}
	}

	void clear()
	{
		for(int j=0; j< m; j++)
			delete[]counters[j];
	}
	
	int insert(const char* key, uint32_t f =1) {
        uint32_t buf_0[d],shift_0[d],val[d];
        for (int i = 0; i < d; i++) {
            uint32_t hashVal = hash[i]->run(key, KEY_LENGTH);
            idx[i] = hashVal % w[0];
            buf_0[i] = counters[0][idx[i] >> 4];  //记下[0]的buf值，之后更新时需要
            shift_0[i] = (idx[i] & lo[0])<< li[0];
            val[i] = (buf_0[i] >> shift_0[i]) & mask[0];   //当下获取[0]的值即可
		}
		if (val[0] != val[1]) {
			uint32_t min_num;
            min_num = (val[0] < val[1]) ?  0 : 1;
			min_value = val[min_num];
            index = idx[min_num];  //2比特的索引位置
            buf0 = buf_0[min_num];
            shift0 = shift_0[min_num];  //记下[0]的shift值
			uint32_t& buf = counters[min_value +1][index >> 4];  //找到真实的32比特的位置
			uint32_t a = index >> min_value; //a是所在层的虚拟位置
			uint32_t shift = (a & lo[min_value]) << li[min_value];  ///再找到真实后进行移位
			value = (buf >> shift) & mask[min_value];  //获取到32比特中的相应比特值，其实是虚拟的计数器值
        	if(value + 1 <= mask[min_value]){
				buf += (1<< shift);
				value = (buf >> shift) & mask[min_value];  //插入结束
				return value;					
			}
			else {  //溢出只有两种情况，0-1/1-2   
            	buf0 += (1 << shift0);  //更新guide，
				min_value = (buf0 >> shift0) & mask[0];  //min_value重新计算更新
				if(min_value == 1){  //0-1
					uint32_t& buf = counters[2][index >> 4];
					uint32_t shift = ((index >> 1) & lo[1]) << li[1];
                    uint32_t  a = 15;
					buf = (4 << shift) + buf & (~(a << shift)); //直接赋值
					value = (buf >> shift) & mask[1];
					return value;	
				}
				else if(min_value ==2){ //1-2
					uint32_t& buf = counters[3][index >> 4];
					uint32_t shift = ((index >> 2) & lo[2]) << li[2];
					// index = (index & 1 ==0)? index +1 : index -1; //==1说明是奇数，==0说明是偶数，注意三目的写法
					// uint32_t adj_val = (counters[0][index >> 4]) >> (index & lo[0] << li[0]) & mask[0];
					// buf = (adj_val == 1) ?  12 << shift : 16 << shift;  //直接赋值                    
					uint32_t  a = 255;
                    buf = (16 << shift) + buf & (~(a << shift)); //直接赋值
					value = (buf >> shift) & mask[2];
					return value;
				}
				
        	}
        }
		else {    
        	min_value = val[0];
			for (int i = 0; i < d; i++){
				uint32_t& buf = counters[min_value +1][idx[i] >> 4];  //找到真实的32比特的位置
				uint32_t a = idx[i] >> min_value; //a是所在层的虚拟位置
				uint32_t shift = (a & lo[min_value]) << li[min_value];  ///再找到真实后进行移位
				value = (buf >> shift) & mask[min_value];  //获取到32比特中的相应比特值
				if(value + 1 < mask[min_value]){
					buf += (1<< shift);  //更新加
					value = (buf >> shift) & mask[min_value];
				}
				else {  
					buf_0[i] += (1 << shift_0[i]);  
					min_value += 1;  
					if(min_value ==1){ //0-1
						uint32_t& buf = counters[2][idx[i] >> 4];
						uint32_t shift = ((idx[i] >> 1) & lo[1]) << li[1];
                    	uint32_t  a = 15;
                    	buf = (4 << shift) + buf & (~(a << shift)); //直接赋值
						value = (buf >> shift) & mask[1];
					}
					else if(min_value == 2){ //1-2
						uint32_t& buf = counters[3][idx[i] >> 4];
						uint32_t shift = ((idx[i] >> 2) & lo[2]) << li[2];
						// idx[i] = (idx[i] & 1 ==0)? idx[i] +1 : idx[i] -1; //==1说明是奇数，==0说明是偶数，注意三目的写法
						// uint32_t adj_val = (counters[0][idx[i] >> 4]) >> (idx[i] & lo[0] << li[0]) & mask[0];
						// buf = (adj_val == 1) ?  12 << shift : 16 << shift;  //分情况去赋值                   
                    	uint32_t a = 255;
						buf = (16 << shift) + buf & (~(a << shift)); //直接赋值
						value = (buf >> shift) & mask[2];
					}
					
				}
			}
		}

    }

	int swap_insert(const char * swap_key, uint32_t swap_val){  //这种情况，我们直接根据流的大小去赋值，而不是去更新
        for (int i = 0; i<d ;i++){
            uint32_t hashVal = hash[i]->run(swap_key, KEY_LENGTH);
            idx[i] = hashVal % w[0];
            uint32_t& buf0 = counters[0][idx[i] >> 4];  //第一层除以16
            if (swap_val <= 3){
                buf0 = (0 << shift0);  //赋值为0
                min_value = (buf0 >> shift0) & mask[0];  //均为0
				uint32_t& buf = counters[1][idx[i] >> 4];  //第二层的位置
				uint32_t shift0 = (idx[i] & lo[0]) << li[0];  //index % counter_per_int * counter_len;
                uint32_t  a = 3;
                buf = (swap_val << shift0) + buf & (~(a << shift0)); //直接赋值
				value = swap_val;
            }
            else if(swap_val <= 15){
				buf0 = (1 << shift0); //给两个位置的计数器赋值
				min_value = (buf0 >> shift0) & mask[0];   //两个计数器的值，均为1
				uint32_t& buf = counters[2][idx[i] >> 4];
				idx[i] = idx[i] >> 1;
 				uint32_t shift = (idx[i] & lo[1]) << li[1];  //此idx是4比特的虚拟索引位置
                uint32_t  a = 15;
                buf = (swap_val << shift) + buf & (~(a << shift)); //直接赋值
				value = swap_val;
			}
            else if (swap_val <= 255){
				buf0 = (2 << shift0); //赋值
				min_value = (buf0 >> shift0) & mask[0];  //均为2
				uint32_t& buf = counters[3][idx[i] >> 4];
				idx[i] = idx[i] >> 2;
				uint32_t shift = (idx[i] & lo[2]) << li[2];
                uint32_t  a = 255;
                buf = (swap_val << shift) + buf & (~(a << shift)); //直接赋值
				value = swap_val;
			}
        }
        return value;

	}


	int query0(const char * key){
    uint32_t buf_0[d],shift_0[d],val[d];
    uint32_t  val0 = 0;
	uint32_t value = UINT32_MAX;
	for (int i = 0; i < d; i++) {
		uint32_t hashVal = hash[i]->run(key, KEY_LENGTH);
		idx[i] = hashVal % w[0];
		buf_0[i] = counters[0][idx[i] >> 4];  //记下[0]的buf值，之后更新时需要
		shift_0[i] = (idx[i] & lo[0])<< li[0];
		val[i] = (buf_0[i] >> shift_0[i]) & mask[0];   //当下获取[0]的值即可
		if(val[i] ==0){ //在计数第一层，没有其它流的影响
			uint32_t buf = counters[1][idx[i] >> 4];
        	uint32_t shift = (idx[i] & lo[0]) << li[0];
        	val0 = (buf >> shift) & mask[0];  //获取两个val0
		}
		else if(val[i] ==1){ //在计数层的第二层，会有相邻位置的影响
			uint32_t buf = counters[2][idx[i] >> 4];
			uint32_t a = idx[i] / 2;
			uint32_t shift = (a & lo[1]) << li[1];
			val0 = (buf >> shift) & mask[1];		
    	}

		else if(val[i] ==2){
			uint32_t buf = counters[3][idx[i] >> 4];
			uint32_t a = idx[i] >> 2;
			uint32_t shift = (a & lo[2]) << li[2];
            val0 = (buf >> shift) & mask[2];
    	}
        value = (val0 < value)? val0 : value;
	}
    return value;	
	}






	int query(const char * key){
    uint32_t buf_0[d],shift_0[d],val[d];
    uint32_t  val0 = 0;
	for (int i = 0; i < d; i++) {
		uint32_t hashVal = hash[i]->run(key, KEY_LENGTH);
		idx[i] = hashVal % w[0];
		buf_0[i] = counters[0][idx[i] >> 4];  
		shift_0[i] = (idx[i] & lo[0])<< li[0];
		val[i] = (buf_0[i] >> shift_0[i]) & mask[0];   
		if(val[i] ==0){ 
			uint32_t buf = counters[1][idx[i] >> 4];
        	uint32_t shift = (idx[i] & lo[0]) << li[0];
        	val0 = (buf >> shift) & mask[0];  
		}
		else if(val[i] ==1){ 
			uint32_t buf = counters[2][idx[i] >> 4];
			uint32_t a = idx[i] / 2;
			uint32_t shift = (a & lo[1]) << li[1];
			val0 = (buf >> shift) & mask[1];		
    	}

		else if(val[i] ==2){
			uint32_t buf = counters[3][idx[i] >> 4];
			uint32_t a = idx[i] >> 2;
			uint32_t shift = (a & lo[2]) << li[2];
            val0 = (buf >> shift) & mask[2];
    	}
        uint32_t value = UINT32_MAX;
        value = (val0 < value)? val0 : value;
	}
    return value;	
	}

	
	
};


#endif



