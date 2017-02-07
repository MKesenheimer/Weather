#include "Interface.h"

Interface::Interface(const char* filename) {
    file.open(filename, std::ios::in);
    //std::cout<<"opening file "<<filename<<std::endl;
    //std::cout<<std::endl;
}

//empty constructor should be followed by Interfaceopen
Interface::Interface() {}
void Interface::Interfaceopen(const char* filename) {
    file.open(filename, std::ios::in);
}

Interface::~Interface() {
    file.close();
    //std::cout<<std::endl;
    //std::cout<<"closing Interface file "<<std::endl;
}

int Interface::getBlockSize(std::string blockname) {
    readBlock(blockname);
    int n = block.size();
    block.clear();
    return n;
}

std::string Interface::getScalarEntry(std::string blockname) {
    readBlock(blockname);
    std::vector<std::string> entry = block[0];
    block.clear();
    return entry[0];
}

std::string Interface::getVectorEntry(std::string blockname, int n) {
    std::string temp = std::string();
    readBlock(blockname);
    for(int i=0; i<(int)block.size(); i++) {
        std::vector<std::string> entry = block[i];
        //std::cout<<entry[0]<<std::endl;
        std::string::size_type sz;
        int p = std::stoi(entry[0],&sz);
        if(p == n) {
            //fill temp with everything what comes after the first column
            for(int j=1; j<(int)entry.size(); j++) {
                if(j!= (((int)entry.size())-1) ) {
                    temp = temp + entry[j] + " ";
                } else {
                    temp = temp + entry[j];
                }
            }
        }
    }
    block.clear();
    return temp;
}

std::string Interface::getMatrixEntry(std::string blockname, int n, int m) {
    std::string temp = std::string();
    readBlock(blockname);
    for(int i=0; i<(int)block.size(); i++) {
        std::vector<std::string> entry = block[i];
        std::string::size_type sz;
        int p = std::stoi(entry[0],&sz);
        int q = std::stoi(entry[1],&sz);
        if(p == n and q == m) {
            //fill temp with everything what comes after the second column
            for (int j=2; j<(int)entry.size(); j++) {
                if(j!= (((int)entry.size())-1) ) {
                    temp = temp + entry[j] + " ";
                } else {
                    temp = temp + entry[j];
                }
            }
        }
    }
    block.clear();
    return temp;
}

void Interface::readBlock(std::string blockname) {
    file.clear();
    file.seekg(0, std::ios::beg);
    //std::cout<<"blockname = "<<blockname<<std::endl;
    blockname = finalizeString(blockname);
    //std::cout<<"finalized blockname = "<<blockname<<std::endl;
    std::vector<std::string> blockContent;

    //loop over lines
    while(file.good()) {
        std::string line;
        //read file line by line
        std::getline(file, line);
        line = finalizeString(line);
        //find BLOCK keyword
        std::size_t pos = line.find("BLOCK");
        if(pos!=std::string::npos) {
            //search line for blockname
            std::vector<std::string> lineVec = strToVec(line);
            if(lineVec.size()!=0) {
                if(lineVec[0].find("BLOCK")!=std::string::npos and blockname.compare(lineVec[1])==0) {
                    //now we are at the beginning of the block
                    bool inblock = true;
                    do {
                        //read the next line
                        std::getline(file, line);
                        //the block is terminated with '#', ' '
                        //or with the next keyword
                        std::size_t pos1 = line.find("BLOCK");
                        std::size_t pos2 = line.find("END");
                        if(line=="#" or line==" " or line=="" or
                            pos1!=std::string::npos or pos2!=std::string::npos) {
                            inblock = false;
                        } else {
                            line = finalizeString(line);
                            //fill blockContent if line is not empty
                            if(line!="") {
                                //std::cout<<line<<std::endl;
                                blockContent.push_back(line);
                            }
                        }
                    } while(inblock and !file.eof());
                }
            }
        }
    }
    block.clear();
    for(int i=0; i<(int)blockContent.size(); i++) {
        //std::cout<<blockContent[i];
        std::vector<std::string> entry = strToVec(blockContent[i]);
        block.push_back(entry);
    }
}

std::string Interface::readNextString(std::string str, std::size_t *pos) {
    std::string temp = std::string();
    *pos = std::string::npos;
    if(str!="" or str!=std::string()) {
        std::size_t n = str.find_first_not_of(" ");
        if(n!=std::string::npos) {
            std::size_t m = str.find_first_of(" ", n);
            temp = str.substr(n, m!=std::string::npos ? m-n : m);
            *pos = n;
        }
    }
    return temp;
}

std::string Interface::finalizeString(std::string str) {
    std::string out;
    if(str!="") {
        //erase what comes after '#'
        std::size_t pos = str.find("#");
        if(pos!=std::string::npos) {
            out = str.substr(0, pos);
        } else {
            out = str;
        }
        //out to upper case
        //std::for_each(out.begin(), out.end(), convertToUpper());
        //std::cout<<out<<std::endl;
        return out;
    } else {
        return std::string();
    }
}

