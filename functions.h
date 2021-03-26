//Tina de Jong
//cdejong2
//Narayani Wagle
//nwagle1

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <limits.h>
#include <queue>

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::map;
using std::vector;
using std::queue;

class Cache {
    public:
        Cache(int sets, int blocks, int bytes, string alloc, string thru, string evict);
        void processLine(string line);
        void print();
        void trace();
    private:
        int numSets;
        int numBlocks;
        int numBytes;
        string writeAlloc;
        string writeThru;
        string evictions;

        string ls;
        uint32_t tag;
        uint32_t index;

        map<uint32_t, vector<uint32_t>> cache;
        map<uint32_t, vector<int>> lru;
        map<uint32_t, queue<int>> fifo;
        

        int cycles;
        int storehits;
        int storemisses;
        int loadhits;
        int loadmisses;

        int offsetBits;
        int indexBits;
        int tagBits;

        int isPowerOfTwo(int num);
        bool checkHit(vector<uint32_t> target);
        void miss(vector<uint32_t> target);
        vector<int> updateLRU(int position);
};

#endif