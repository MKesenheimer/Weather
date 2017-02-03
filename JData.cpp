#include "JData.h"

JData::JData(const std::string str) {
    document.Parse(str);
}

JData::~JData() {}

// TODO: Error handling

std::string JData::getCityName() {
    assert(document.HasMember("name"));
    assert(document["name"].IsString());
    return document["name"].GetString();
}


double JData::getTemperature() {
    assert(document.HasMember("main"));
    assert(document["main"].IsObject());
    auto m = document["main"].GetObject();
    assert(m.HasMember("temp"));
    assert(m["temp"].IsNumber());
    return m["temp"].GetDouble();
}

double JData::getPressure() {
    assert(document.HasMember("main"));
    assert(document["main"].IsObject());
    auto m = document["main"].GetObject();
    assert(m.HasMember("pressure"));
    assert(m["pressure"].IsNumber());
    return m["pressure"].GetDouble();
}

double JData::getHumidity() {
    assert(document.HasMember("main"));
    assert(document["main"].IsObject());
    auto m = document["main"].GetObject();
    assert(m.HasMember("humidity"));
    assert(m["humidity"].IsNumber());
    return m["humidity"].GetDouble();
}

double JData::getWindSpeed() {
    assert(document.HasMember("wind"));
    assert(document["wind"].IsObject());
    auto m = document["wind"].GetObject();
    assert(m.HasMember("speed"));
    assert(m["speed"].IsNumber());
    return m["speed"].GetDouble();
}

double JData::getWindDirection() {
    assert(document.HasMember("wind"));
    assert(document["wind"].IsObject());
    auto m = document["wind"].GetObject();
    if(m.HasMember("deg")) {
        assert(m["deg"].IsNumber());
        return m["deg"].GetDouble();
    } else {
        return std::numeric_limits<float>::quiet_NaN();
    }
}

double JData::getClouds() {
    assert(document.HasMember("clouds"));
    assert(document["clouds"].IsObject());
    auto m = document["clouds"].GetObject();
    assert(m.HasMember("all"));
    assert(m["all"].IsNumber());
    return m["all"].GetDouble();
}

std::vector<double> JData::getData() {
    std::vector<double> data;
    data.push_back(getTemperature());
    data.push_back(getPressure());
    data.push_back(getHumidity());
    data.push_back(getWindSpeed());
    data.push_back(getWindDirection());
    data.push_back(getClouds());
    return data;
}