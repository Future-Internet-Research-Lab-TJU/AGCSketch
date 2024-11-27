#ifndef _ASSAULT_H_
#define _ASSAULT_H_

#include "param.h"
#include "BOBHash32.h"
#include<stdlib.h>
#include <cstring>
#include <iostream>



template<int bucket_num>   
class AssaultPart
{
public:
    
	uint32_t min_idx;
	uint32_t min_val = UINT32_MAX;
	Bucket buckets[bucket_num];
    BOBHash32 * hash;
	AssaultPart(){
        hash = new BOBHash32(750);
    }
	~AssaultPart(){clear(); }

	void clear()
	{
		memset(buckets, 0, sizeof(Bucket) * bucket_num);
		delete hash;
	}
		

/* insertion */
	int insert(const char * key, uint32_t f )  
	{
		// min_val = UINT32_MAX;
		uint32_t index = hash->run(key, KEY_LENGTH) % bucket_num;
		uint32_t pos;   
		for(int i =0; i< NUMBER_Of_TRAVERSALS; i++)
		{
			(index + i < bucket_num) ? pos = (index + i) : (index + i -bucket_num -1);
			if(buckets[pos].key == string(key ,13)){   
				buckets[pos].value += f; 
				return 0;  
			}
			else if(buckets[pos].value == 0){ 
				buckets[pos].key = string(key ,13);   
				buckets[pos].value = f;
				return 0;
			}
			else if(buckets[pos].value < min_val){   
				min_idx = pos;
				min_val = buckets[pos].value;
				return 1;  
			} 
		}
		 
	}


/* query */
	int query(const char* key)
	{		
		uint32_t index = hash->run(key, KEY_LENGTH) % bucket_num; 
		uint32_t pos;
		for(int i = 0; i < NUMBER_Of_TRAVERSALS; i++){
			(index + i < bucket_num) ? pos = (index+ i) : (index + i -bucket_num -1);
			if(buckets[pos].key == string(key ,13))   
				return buckets[pos].value;			
		}
		return 0; 
	}
    


};
#endif
