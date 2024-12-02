# AGC Sketch: An effective and accurate per-flow measurement to adapt flow size distribution

---
### Paper

__Zhuo Li,Ziyu Liu,Jindian Liu, Yu Zhang ,Teng Liang, Kaihua Liu__ 
AGC Sketch: An effective and accurate per-flow measurement to adapt flow size distribution
__Computer Communications 221 (2024)__ 

---
### Files
- agc.h: the overall framework of data structure
- assault.h: the assault part of AGC Sketch
- Guide_Candidate_Path.h, guide_candidate.h: the guide and candidate part of AGC Sketch
- param.h: initialization of the structure
- BOBHash32.h: the hash function
- heavy_change.cpp, heavy_change0.cpp: the heavy change test
- heavy_hitter.cpp: the heavy hitter test
- AGCSketch.cpp: the frequency test
- throughput.cpp: the throughput test and the result
---

### Compile and Run the examples
AGCSketch is implemented with C++. We show how to compile the examples on
Ubuntu with g++ and make.

#### Requirements
- Ensure __g++__ and __make__ are installed.  Our experimental platform is
  equipped with Ubuntu 20.04

  - Prepare the test files.
   - You can use Real-life Transactional,caida,Zipf distribution and so on to test its performance.


   #### Compile
- Compile examples with make

```
    $ make frequency
    $ make heavy_change
    $ make heavy_hitter
```

- The g++ compiler should be suitable 


#### Run
- Run the examples, and the program will output some statistics. 

```
    $ ./agc
```
