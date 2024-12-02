#ifndef _guide_candidate_H
#define _guide_candidate_H


#include<stdlib.h>
#include "../common/BOBHash32.h"
#include "param.h"
using namespace std;

template<int guide_candidate_mem>
class Guide_Candidate_Part 
{  	
private:
    uint32_t adj_val,shift0,shift,buf0,buf;
    uint32_t *counters[m];  
	uint32_t idx[d],val[d],shift_0[d];
	uint32_t w[m];
	uint32_t min_value = UINT32_MAX;
    BOBHash32 * hash[d];
public:
    uint32_t value;
Guide_Candidate_Part() { init();}
~Guide_Candidate_Part() { clear(); }  

    void init(){  
		uint32_t in_w = guide_candidate_mem / 4 / 4;  
		for(int i =0; i < m; i++)
		{
			counters[i]= new uint32_t[in_w];  
			if(i < 2)
				w[i]= in_w <<4;
			else 
				w[i]= in_w <<(5-i);
			memset(counters[i], 0, in_w * sizeof(uint32_t));		
		}	
		for(int i=0; i<d; i++)
        {
			hash[i] = new BOBHash32(i + 1000);
		}
	}

	void clear()
	{
		for(int j=0; j< m; j++)
			delete[]counters[j];
	}
	

	int insert(const char* key, uint32_t f ){
	uint32_t index;
	for(int i=0; i < d; i++){
		uint32_t hashVal = hash[i]->run(key, KEY_LENGTH);
		idx[i] = hashVal % w[0]; 
        buf0 = counters[0][idx[i] >> cpw[0]];  
		val[i] = (buf0 >> shift_0[i]) & mask[0];   
		if(val[i] < min_value)
		{
			min_value = val[i];
			index = idx[i];
			shift0 = shift_0[i]; 
		}
	}	
	if(min_value == 0){
		switch(f)
		{
			case 0 ... 3: {
                uint32_t &buf = counters[1][index >> cpw[0]];
                uint32_t shift = index & lo[0] << li[0];
                buf += (f << shift); 
				uint32_t value = (buf >> shift) & mask[0];
                return 0;
                break;
            }
			case 4 ... 15: {
                uint32_t &buf = counters[2][index >> cpw[1]];  
                uint32_t shift = index & lo[1] << li[1]; 
                buf += (f << shift);
				uint32_t value = (buf >> shift) & mask[1];
                return 0;
                break;
            }
			case 16 ... 255: {
                uint32_t &buf = counters[3][index >> cpw[2]];  
                uint32_t shift = index & lo[2] << li[2];
                buf += (f << shift);
				uint32_t value = (buf >> shift) & mask[2];
                return 0;
                break;
            }
		}
	}
	else{
		uint32_t& buf = counters[min_value][index >> cpw[min_value - 1]]; 
		uint32_t shift = index & lo[min_value - 1] << li[min_value - 1];  
		uint32_t value = (buf >> shift) & mask[min_value - 1];  
		//做是否溢出的判断
		if(value + f < mask[min_value - 1]){
			buf += (f << shift);   
			uint32_t value = (buf >> shift) & mask[min_value - 1]; 
			return 0;
		} 
		else{
			index & 1== 1 ? adj_val= (counters[0][(index +1) >> cpw[0]]) >> ((index +1) & lo[0] << li[0]) & mask[0] : (counters[0][(index- 1) >> cpw[0]]) >> ((index -1) & lo[0] << li[0]) & mask[0]; //求取[0]相邻的计数器值
			if(min_value ==2 && min_value == adj_val){  
				uint32_t temp = value ;
				//buf -= (mask[0] << shift);  
				buf0 += (1 << shift0);  
				uint32_t shift1 = index & lo[2] << li[2];
				uint32_t& buf = counters[3][index >> cpw[2]];
				buf += (temp - mask[0]) << shift1;  
				uint32_t value = (buf >> shift) & mask[2];
				return 0;
			}
			else{ //全清（可以不清）
				uint32_t temp = value;  
				buf0 += (1 << shift0);  
				uint32_t& buf = counters[min_value +1][index >> cpw[min_value]];
				uint32_t shift1 = index & lo[min_value] << li[min_value];
				buf += (temp << shift);  
				uint32_t value = (buf >> shift1) & mask[min_value];
				return 0;
			}
		}
	}
	cout << value << endl;
	}

	
	int query(const char * key){
	int index;
	for(int i=0; i <d; i++){
		uint32_t hashVal = hash[i]->run(key, KEY_LENGTH);
		idx[i] = hashVal % w[0]; 
		buf0 = counters[0][idx[i] >> cpw[0]]; 
		shift_0[i] = idx[i] & lo[0] << li[0];
		val[i] = (buf0 >> shift0) & mask[0];  
		if(val[i] < min_value)
			min_value = val[i];
			index = idx[i];
			shift0 = shift_0[i];
	}
	switch (min_value)
	{
        case 0:
            return 0;
        case 1:
		buf = counters[1][index >> cpw[0]]; 
		shift = index & lo[0] << li[0];  
		value = (buf >> shift) & mask[0]; 
		return value;
		break;  
	case 2: 
		buf = counters[2][index >> cpw[1]]; 
		shift = index & lo[1] << li[1];  
		value = (buf >> shift) & mask[1]; 
		index & 1==1 ? adj_val= (counters[0][(index+1) >> cpw[0]]) >> ((index +1) & lo[0] << li[0]) & mask[0] : (counters[0][(index- 1) >> cpw[0]]) >> ((index -1) & lo[0] << li[0]) & mask[0]; 
		if(min_value = adj_val){
			value = value- 4;
		}
		return value;
		break;  
	case 3:
		buf = counters[3][index >> cpw[2]]; 
		shift = index & lo[2] << li[2];  
		value = (buf >> shift) & mask[2]; 
		uint32_t cnt = 0;
		for (int i = 0; i < 4; i++)
		{
			if ((3 & (buf >> (i << 1))) == 3)
				cnt++;
		}
		cnt--;
		uint32_t extral = cnt << 4;
		return value - extral;
		break;  
	}
	}
};

//Guide_Candidate_Part<450> guide_candidate_part;

#endif



