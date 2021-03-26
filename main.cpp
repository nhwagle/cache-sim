//CSF Assignment 3
//Tina de Jong - cdejong2
//Narayani Wagle - nwagle1

#include "functions.h"

using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::cin;
using std::map;
using std::vector;

// isPowerOfTwo is a helper function to determine if a number is a power of two, and if so, what the power is
// Input: int num - some integer value
// Returns: int - the power of two for the num, -1 if not a power of two
int isPowerOfTwo(int num) {
    if ((num != 0) && ((num &(num - 1)) == 0)) {
        return log2(num);
    } else {
        return -1;
    }
}

// invalidConversion checks if a processed integer argument contained non-integer values
// Input: int num - predicted integer input from argument, string input - string argument
// Return bool - True if string to int conversion was incorrect, False if correct
bool invalidConversion(int num, string input){
    if(to_string(num).length() == input.length()){
        return false;
    } else{
        return true;
    }
}

string selectErrorMessage(int e){
    if(e == 0){
        return "Non-integer value was provided for sets, blocks, and/or bytes";
    } else if(e == 1){
        return "Integer value provided for sets, blocks, and/or bytes is too low";
    } else if(e == 2){
        return "Integer value provided for sets, blocks, and/or bytes is not a power of two";
    } else if(e == 3){
        return "4th argument must be 'write-allocate' or 'no-write-allocate'";
    } else if(e == 4){
        return "5th argument must be 'write-through' or 'write-back'";
    } else if(e == 5){
        return "no-write-allocate and write-back together is not a valid set of arguments";
    } else{
        return "6th argument must be 'lru' or 'fifo'";
    }
}

int main(int argc, char **argv) {

    // check if there are too few arguments
    if (argc < 6) {
        cout << "Error: not enough arguments" << endl;
        exit(1);
    }

    // process command line arguments
    int numSets = atoi(argv[1]);
    int numBlocks = atoi(argv[2]);
    int numBytes = atoi(argv[3]);
    string writeAlloc = argv[4];
    string writeThru = argv[5];
    string evictions = argv[6];

    //check for various invalid arguments
    try {
        if(invalidConversion(numSets, argv[1]) || invalidConversion(numBlocks, argv[2]) || invalidConversion(numBytes, argv[3])){
            throw 0;
        }
        if (numSets < 1 || numBlocks < 1 || numBytes < 4) {
            throw 1;
        }
        if (isPowerOfTwo(numSets) == -1 || isPowerOfTwo(numBlocks) == -1 || isPowerOfTwo(numBytes) == -1) {
            throw 2;
        }
        if (writeAlloc != "write-allocate" && writeAlloc != "no-write-allocate") {
            throw 3;
        }
        if (writeThru != "write-through" && writeThru != "write-back") {
            throw 4;
        }
        if (writeAlloc == "no-write-allocate") {
            if (writeThru == "write-back") {
                throw 5;
            }
        }
        if (evictions != "lru" && evictions != "fifo") {
            throw 6;
        }
    }
    catch(int e) {
        cout << "Invalid Argument Error: " << selectErrorMessage(e) << endl;
        exit(1);
    }

    Cache cache(numSets, numBlocks, numBytes, writeAlloc, writeThru, evictions);

    // loop through trace file lines
    while (cin.eof() == 0) {
        string line;
        getline(cin, line);

        // ignores blank lines - possibly at end of file
        if(line.length() < 1){
            break;
        }
        // process one line of input, update stats
        cache.processLine(line);
    }
    // display resulting stats
    cache.print();
    return 0;
}