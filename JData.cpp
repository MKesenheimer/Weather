#include "JData.h"

JData::JData(const std::string str) {
    document.Parse(str);
    stored = str;
}

JData::~JData() {}

std::string JData::getCityName() {
    if(document.HasMember("name") && document["name"].IsString()) {
        return document["name"].GetString();
    } else {
        std::cout << "Warning: object \"name\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return "";
    }
    
}

double JData::getTemperature() {
    if(document.HasMember("main") && document["main"].IsObject()) {
        auto m = document["main"].GetObject();
        if(m.HasMember("temp") && m["temp"].IsNumber()) {
            return m["temp"].GetDouble();
        } else {
            std::cout << "Warning: object \"temp\" not found." << std::endl;
            std::cout << stored << std::endl;
            isValidDataSet = false;
            return 0;
        }
    } else {
        std::cout << "Warning: object \"main\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return 0;
    }
}

double JData::getPressure() {
    if(document.HasMember("main") && document["main"].IsObject()) {
        auto m = document["main"].GetObject();
        if(m.HasMember("pressure") && m["pressure"].IsNumber()) {
            return m["pressure"].GetDouble();
        } else {
            std::cout << "Warning: object \"pressure\" not found." << std::endl;
            std::cout << stored << std::endl;
            isValidDataSet = false;
            return 0;
        }
    } else {
        std::cout << "Warning: object \"main\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return 0;
    }
}

double JData::getHumidity() {
    if(document.HasMember("main") && document["main"].IsObject()) {
        auto m = document["main"].GetObject();
        if(m.HasMember("humidity") && m["humidity"].IsNumber()) {
            return m["humidity"].GetDouble();
        } else {
            std::cout << "Warning: object \"humidity\" not found." << std::endl;
            std::cout << stored << std::endl;
            isValidDataSet = false;
            return 0;
        }
    } else {
        std::cout << "Warning: object \"main\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return 0;
    }
}

double JData::getWindSpeed() {
    if(document.HasMember("wind") && document["wind"].IsObject()) {
        auto m = document["wind"].GetObject();
        if(m.HasMember("speed") && m["speed"].IsNumber()) {
            return m["speed"].GetDouble();
        } else {
            std::cout << "Warning: object \"speed\" not found." << std::endl;
            std::cout << stored << std::endl;
            isValidDataSet = false;
            return 0;
        }
    } else {
        std::cout << "Warning: object \"wind\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return 0;
    }
}

double JData::getWindDirection() {
    if(document.HasMember("wind") && document["wind"].IsObject()) {
        auto m = document["wind"].GetObject();
        if(m.HasMember("deg") && m["deg"].IsNumber()) {
            return m["deg"].GetDouble();
        } else {
            std::cout << "Warning: object \"deg\" not found." << std::endl;
            std::cout << stored << std::endl;
            // isValidDataSet is not set to false here because on some days
            // with no wind the wind direction is not available per se.
            return std::numeric_limits<double>::quiet_NaN();
        }
    } else {
        std::cout << "Warning: object \"wind\" not found." << std::endl;
        std::cout << stored << std::endl;
        return std::numeric_limits<double>::quiet_NaN();
    }
}

double JData::getClouds() {
    if(document.HasMember("clouds") && document["clouds"].IsObject()) {
        auto m = document["clouds"].GetObject();
        if(m.HasMember("all") && m["all"].IsNumber()) {
            return m["all"].GetDouble();
        } else {
            std::cout << "Warning: object \"all\" not found." << std::endl;
            std::cout << stored << std::endl;
            isValidDataSet = false;
            return std::numeric_limits<double>::quiet_NaN();
        }
    } else {
        std::cout << "Warning: object \"clouds\" not found." << std::endl;
        std::cout << stored << std::endl;
        isValidDataSet = false;
        return std::numeric_limits<double>::quiet_NaN();
    }
}

std::vector<double> JData::getData() {
    isValidDataSet = true;
    std::vector<double> data;
    data.push_back(getTemperature());
    data.push_back(getPressure());
    data.push_back(getHumidity());
    data.push_back(getWindSpeed());
    data.push_back(getWindDirection());
    data.push_back(getClouds());
    if(isValidDataSet) {
        return data;
    } else {
        return std::vector<double>();
    }
}