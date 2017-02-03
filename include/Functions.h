#ifndef Functions_H
#define Functions_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <limits> 
#include <sstream>
#include <algorithm>

void strToChar(const std::string tmp, char (&chars)[1024]);
bool is_number(const std::string& s);
std::vector<std::string> strToVec(std::string str);
std::string readNextString(std::string str, std::size_t *pos);
long strToDouble(std::string str);
long double strToLongDouble(std::string str);
std::string intToStr(int a);
std::string doubleToStr(double a);

#endif
