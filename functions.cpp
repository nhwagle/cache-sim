//Tina de Jong
//cdejong2
//Narayani Wagle
//nwagle1

//cache functions

#include "functions.h"

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;
using std::queue;

//Cache constructor
Cache::Cache(int sets, int blocks, int bytes, string alloc, string thru, string evict) : numSets(sets), numBlocks(blocks), numBytes(bytes), writeAlloc(alloc), writeThru(thru), evictions(evict) {
    // initialize cache for each index with a vector of length blocks
    for (uint32_t i = 0; i < (uint32_t)numSets; i++) {
        vector<uint32_t> x;
        cache[i] = x;

        // intialize structure to keep track of which blocks to evict
        if(evictions == "lru") {
            vector<int> y;
            for (int j = 0; j < numBlocks; j++) {
                y.push_back(j);
            }
            lru[i] = y;
        } else {
            queue<int> q;
            fifo[i] = q;
        }
    }

    // initialize all stats to zero
    cycles = 0;
    storehits = 0;
    storemisses = 0;
    loadhits = 0;
    loadmisses = 0;

    // calculate tag, index, and offset bits
    offsetBits = isPowerOfTwo(numBytes);
    indexBits = isPowerOfTwo(numSets);
    tagBits = 32 - (offsetBits + indexBits);

    // initialize store/load, tag and index
    ls = "";
    tag = 0;
    index = 0;
}

//Cache member functions

// isPowerOfTwo is a helper function to determine if a number is a power of two, and if so, what the power is
// Input: int num - some integer value
// Returns: int - the power of two for the num, -1 if not a power of two
int Cache::isPowerOfTwo(int num) {
    if ((num != 0) && ((num & (num - 1)) == 0)) {
        return log2(num);
    }
    else {
        return -1;
    }
}

// processLine breaks a line from the trace file into its parts (store/load token, address, tag, index), updates local variables
// Input: string line - one line from the trace file
void Cache::processLine(string line)
{
    // determine if store or load
    ls = line.substr(0, 1);

    // calculate index and tag from address
    string stringAddress = line.substr(4, 8);
    uint32_t address = strtoul(stringAddress.c_str(), NULL, 16);
    tag = address >> (32 - tagBits);
    uint32_t temp = address << tagBits;
    index = temp >> (tagBits + offsetBits);

    // update cache and/or stats with info from the line in trace
    trace();
}

// trace processes the current trace line, checks if cache hit and calls miss if not a hit
void Cache::trace()
{
    // select set of blocks that correspond to index in address of line
    vector<uint32_t> target = cache[index];

    // check if hit or miss, process accordingly
    bool notfound = checkHit(target);
    if (notfound) {
        miss(target);
    }
}

// checkHit determines if the current line in a trace caused a cache hit, updates cache if hit
// Input: vector<uint32_t> targetSet - a vector of uint32_t representing the set in the cache at index specified in line of trace
// Return - bool representing if a hit was not found --> True if no hit, False if hit
bool Cache::checkHit(vector<uint32_t> targetSet)
{
    // loop through each block at target index set in cache
    for (int i = 0; i < (int)targetSet.size(); i++) {

        // hit found
        if (targetSet.at(i) == tag) {

            // update structure to keep track of eviction priority for LRU cache
            if(evictions == "lru"){
                lru[index] = updateLRU(i);
            }
            
            // update stats accordingly for store or load
            if (ls.compare("s") == 0) {
                storehits++;
                if (writeThru.compare("write-through") == 0) {
                    cycles += 101; // store hit, WA/WT or NWA/WT
                } else {
                    cycles++; // store hit, WA/WB
                }
            }
            else {
                loadhits++;
                cycles++; // load hit, WA/WB or WA/WT or NWA/WT
            }
            return false;
        }
    }
    // reach here only if no hit
    return true;
}

// miss modifies the cache in the case of a cache miss, handles evictions if necessary
// Input: vector<uint32_t> targetSet - a vector of uint32_t representing the set in the cache at index specified in line of trace
void Cache::miss(vector<uint32_t> targetSet)
{
    // update stats if load
    if(ls == "l"){
        loadmisses++;
        cycles += 25*numBytes + 1; // load miss, WA/WB or WA/WT or NWA/WT
    }

    // update cache and stats if load or write-allocate
    if (writeAlloc.compare("write-allocate") == 0 || ls == "l") {

        // update stats for store write-through and store write-back conditions
        if(ls == "s" && writeThru == "write-through"){
            storemisses++;
            cycles += 25*numBytes + 101; // store miss, WA/WT
        } else if(ls == "s" && writeThru == "write-back"){
            storemisses++;
            cycles += 25*numBytes + 1; // store miss, WA/WB
        }

        // process eviction
        if ((int)targetSet.size() == numBlocks) {

            // evict block based on lru or fifo rules
            if(evictions == "lru"){
                // identify the index of the least recently used block in the target index set in cache
                int findLRUind = lru[index].at(lru[index].size() - 1);
                targetSet.at(findLRUind) = tag;
            } else{
                // identify index of the oldest block added in target index set in cache
                int findFIFOind = fifo[index].front();

                // update index identified to be the newest in fifo structure
                fifo[index].pop();
                fifo[index].push(findFIFOind);
                targetSet.at(findFIFOind) = tag;
            }

            // update cache with new block
            cache[index] = targetSet;

            // update stats for write-back condition in case of eviction
            if (writeThru.compare("write-back") == 0) {
                cycles += 100 * (numBytes / 4);
            }
        }
        else { // no eviction, add tag to empty block at index set in cache
            // add tag in an empty block in target index set in cache
            targetSet.push_back(tag);
            cache[index] = targetSet;
            int pos = (int)(targetSet.size() - 1);

            // update lru or fifo structures accordingly
            if(evictions == "lru"){
                lru[index] = updateLRU(pos);
            } else{
                fifo[index].push(pos);
            }
            
        }
    }
    else { // update stats for no-write-allocate
        storemisses++;
        cycles += 100; // store miss, NWA/WT
    }
}

// updateLRU modifies the data structure used to keep track of the least recently used block in the cache
// Input: int pos - integer reference to some newly used block position in the cache
// Return: vector<int> representing the updated positions of the least recently used blocked in some set
vector<int> Cache::updateLRU(int pos)
{
    vector<int> inds = lru[index];

    // loop through indices of least recently used blocks at some index in cache to search for index pos
    for (int j = 0; j < (int)inds.size(); j++){
        // index pos found, update index pos location in lru
        if (pos == inds.at(j)){
            inds.erase(inds.begin() + j);
            inds.insert(inds.begin(), pos);
            break;
        }
    }
    return inds;
}

// print displays the statistics from the program
void Cache::print()
{
    cout << "Total loads: " << loadhits + loadmisses << endl;
    cout << "Total stores: " << storehits + storemisses << endl;
    cout << "Load hits: " << loadhits << endl;
    cout << "Load misses: " << loadmisses << endl;
    cout << "Store hits: " << storehits << endl;
    cout << "Store misses: " << storemisses << endl;
    cout << "Total cycles: " << cycles << endl;
}
