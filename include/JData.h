// Interface to json data format
// http://rapidjson.org/index.html

#ifndef JDATA_H
#define JDATA_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <limits> 

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

//using namespace rapidjson;

class JData {
  public:
    JData(const std::string str);
    ~JData();

    std::string getCityName();

    double getTemperature();

    double getPressure();

    double getHumidity();

    double getWindSpeed();

    double getWindDirection();

    double getClouds();
    
    std::vector<double> getData();

  private:
    rapidjson::Document document;
    std::string stored;
    bool isValidDataSet;
};

#endif