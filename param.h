#ifndef _PARAM_H_
#define _PARAM_H_

#include "../common/BOBHash32.h"

#include <x86intrin.h>
#include <string.h>
#include <stdint.h>
#include <random>
#include <string>
#include <memory>
#include <iostream>
#include <cmath>
#include <math.h>

#define NUMBER_Of_TRAVERSALS 3 

#define CONSTANT_NUMBER 2654435761u

#define KEY_LENGTH 13

#define COUNTER_PER_BUCKET 8

#define CalculateBucket(fp) (((fp) * CONSTANT_NUMBER) >> 15)  


#define JUDGE_IF_SWAP(min_counter_val, val) ((val) > ((min_counter_val) * 2))  

#define d 2
#define m 4



uint32_t mask[] = {3, 15, 255};
uint32_t lo [] = {15, 7, 3};
uint32_t li [] = {1, 2, 3};
uint32_t cpw [] = {4, 3, 2};
struct Bucket
{
	string key;
	uint32_t value;
};

struct Header{
    uint8_t version;
    uint32_t source_ipv4;
    uint32_t dest_ipv4;
    uint64_t source_ipv6_high;
    uint64_t source_ipv6_low;
    uint64_t dest_ipv6_high;
    uint64_t dest_ipv6_low;
    uint16_t source_port;
    uint16_t dest_port;
    uint8_t protocol;
};

struct Packet{
    uint32_t source;
    uint32_t dest;
    uint16_t source_port;
    uint16_t dest_port;
    uint8_t protocol;
};

#endif
