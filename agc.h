#ifndef _AGC_H_
#define _AGC_H_

#include "assault.h"
//#include "Guide_Candidate_Part.h"
#include "guide_candidate.h"
#include <unordered_map>
#include <string>
#include <set>

#define HH_THRESHOLD  2489

template<int bucket_num, int tot_memory_in_bytes>
class AGCSketch
{
public:
    int num_light_nonzero = 0;
    int num_heavy_nonzero = 0;
    int card = 0;
    
    static constexpr int assault_mem = bucket_num * 8;  
    static constexpr int guide_candidate_mem = tot_memory_in_bytes - assault_mem;  
    

    AssaultPart<bucket_num> assault_part;
    Guide_Candidate_Part<guide_candidate_mem> guide_candidate_part;

public:
    AGCSketch(){}
    ~AGCSketch(){}
    void clear()              
    {
        assault_part.clear();
        guide_candidate_part.clear();
    }
    
    int insert( const char *key, uint32_t f = 1) {
        string skey= string (key ,13);
        string swap_key;
        int result = assault_part.insert(key, f); 

        switch(result)
        {
            case 0: {
                return 0;
                break;
            }
            case 1: {
                guide_candidate_part.insert(key, 1);
                
                if (guide_candidate_part.value < assault_part.min_val * 2) {
                    return guide_candidate_part.value;
                    break;
                }
                else {
                    swap_key = assault_part.buckets[assault_part.min_idx].key;
                    uint32_t swap_val = assault_part.buckets[assault_part.min_idx].value;  
                    assault_part.buckets[assault_part.min_idx].key = skey;
                    assault_part.buckets[assault_part.min_idx].value = guide_candidate_part.value;  
                    guide_candidate_part.swap_insert(swap_key.c_str(), swap_val);

                   

                    return guide_candidate_part.value;
                    break;
                }
            }

        }
    }

    int query(const char *key)
    {
        int assault_result = assault_part.query(key);
        if(assault_result == 0 ){
            // printf("query = %d\n", guide_candidate_part.query(key));
            return guide_candidate_part.query(key);
        }
        return assault_result;

    }

    void get_heavy_hitters(int threshold, vector<pair<std::string, int> > & results)
    {
        for (int i = 0; i < bucket_num; ++i)  
        {
            string key = assault_part.buckets[i].key;
            int val = assault_part.buckets[i].value;
            //int val = this->query(key.c_str());
            if (val >= threshold) {
                results.push_back(make_pair(key, val));
            }
        }
        
    }

};

#endif
