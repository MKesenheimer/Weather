#include "Functions.h"

void strToChar(const std::string tmp, char (&chars)[1024]) {
    strncpy(chars, tmp.c_str(), sizeof(chars));
    chars[sizeof(chars) - 1] = 0;
}

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

std::vector<std::string> strToVec(std::string str) {
    //extract the entries (ignore whitespaces)
    std::vector<std::string> entry;
    std::string strcopy = str;
    std::size_t pos = 0;
    while(pos!=std::string::npos) {
        std::string temp = readNextString(strcopy, &pos);
        //delete this string in blockContent[i]
        if(temp!="") {
            strcopy = strcopy.substr(pos+temp.length());
            entry.push_back(temp);
        }
    }
    return entry;
}

std::string readNextString(std::string str, std::size_t *pos) {
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

int strToInt(std::string str) {
    //std::cout << str << std::endl;
    std::string::size_type sz;
    if(str!="" or str!=std::string()) {
        int p = std::stoi(str,&sz);
        return p;
    } else {
        return 0;
    }
}

long strToDouble(std::string str) {
    std::string::size_type sz;
    if(str!="" or str!=std::string()) {
        double p = std::stold(str,&sz);
        return p;
    } else {
        return std::numeric_limits<float>::quiet_NaN();
    }
}

long double strToLongDouble(std::string str) {
    std::string::size_type sz;
    if(str!="" or str!=std::string()) {
        long double p = std::stold(str,&sz);
        return p;
    } else {
        return std::numeric_limits<float>::quiet_NaN();
    }
}

std::string intToStr(int a) {
    std::stringstream ss;
    ss << a;
    std::string str = ss.str();
    return str;
}

std::string doubleToStr(double a) {
    std::stringstream ss;
    ss << a;
    std::string str = ss.str();
    return str;
}
