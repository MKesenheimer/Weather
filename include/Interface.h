#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Functions.h"

class Interface {
  public:
    //the constructor
    Interface(const char* filename);
    //empty constructor should be followed by Interfaceopen
    Interface();
    void Interfaceopen(const char* filename);
    //the destructor
    ~Interface();

    //get a single entry of a block
    std::string getScalarEntry(std::string blockname);
    //get the entry of a block with index n
    std::string getVectorEntry(std::string blockname, int n);
    //get the entry of a block with index n and m
    std::string getMatrixEntry(std::string blockname, int n, int m);
    //block size
    int getBlockSize(std::string blockname);

  private:
    //read block from file and store it in a vector string
    void readBlock(std::string blockname);
    
    //read the next substring in str that is not a whitespace and
    //store the position of the substring in pos
    std::string readNextString(std::string str, std::size_t *pos);
  
    //the content of a Interface block
    std::vector< std::vector<std::string> > block;
    std::fstream file;
    //delete comments (beginning with '#' and convert to upper case)
    std::string finalizeString(std::string str);
    //string conversion to upper case
    struct convertToUpper {
        void operator()(char& c) { c = toupper((unsigned char)c); }
    };
};

#endif