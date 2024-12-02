# AGC Sketch: An effective and accurate per-flow measurement to adapt flow size distribution

---
### Paper

__Zhuo Li,Ziyu Liu,Jindian Liu, Yu Zhang ,Teng Liang, Kaihua Liu.__ 
AGC Sketch: An effective and accurate per-flow measurement to adapt flow size distribution.
_Computer Communications 221 (2024) 120-130_

---
### Files
- agc.h: the implementation of AGC Sketch
- assault.h: the assault part of AGC Sketch
- Guide_Candidate_Path.h, guide_candidate.h: the guide and candidate part of AGC Sketch
- param.h: parameters of the AGC Sketch
- BOBHash32.h: the hash functions
- frequency.cpp: flow size estimation
- heavy_change.cpp: heavy changer detection
- heavy_hitter.cpp: heavy hitter detection
- throughput.cpp: throughput
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
    $ make throughput
```

- The g++ compiler should be suitable 


#### Run
- Run the examples, and the program will output some statistics. 

```
    $ ./frequency
    $ ./heavy_change
    $ ./heavy_hitter
    $ ./throughput
```
