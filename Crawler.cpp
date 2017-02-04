// install curl on your system before using this program:
// sudo port install curl
// apt-get install libcurl4-gnutls-dev

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "JData.h"
#include "Curl.h"
#include "Interface.h"

//TODO move to weather.config
//get weather data x times per day
#define CALLSPERDAY 48
//extend data file with current data
#define EXT true
//number of observables
#define NOBS 3

int main(int argc, const char * argv[]) {

    // load config file
    Interface interface("weather.config");
    int ncities = interface.getBlockSize("CITYIDS");
    std::fstream datafile, trainfile;
    std::string filename = std::string("weather_");
    filename.append(intToStr(CALLSPERDAY));
    filename.append(".dat");
    std::cout << "Opening " << filename << std::endl;
    datafile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    
    datafile << "# City IDs: ";
    for (int i = 0; i < ncities; i++) {
        std::string cityID = interface.getVectorEntry("CITYIDS", i);
        datafile << cityID << ", ";
    }
    datafile << std::endl;
    
    std::vector<std::string> ids = {"Tem","Pre","Hum","Spe","Dir","Clo"};
    datafile << "# Observables: Time, ";
    for (int i = 0; i < ncities; i++) {
        for (int j = 0; j < NOBS; j++) {
            datafile << ids[j] << ", ";
        }
    }
    datafile << std::endl;
    //datafile << std::setprecision(1) << std::fixed;

    while (1) {
        
        // add time stamp
        time_t now = time(0);
        //std::string dt = ctime(&now);
        tm *ltm = localtime(&now);
        // time stamp in seconds in respect of one day
        int dstamp = 60*60*(ltm->tm_hour) + 60*(ltm->tm_min) + ltm->tm_sec;
        if(EXT) datafile << dstamp << " ";
        
        // print date and time
        std::cout << "Date: " << ltm->tm_mday << ".";
        std::cout << 1 + ltm->tm_mon << ".";
        std::cout << 1900 + ltm->tm_year << std::endl;
        std::cout << "Time: "<< ltm->tm_hour << ":";
        std::cout << 1 + ltm->tm_min << ":";
        std::cout << 1 + ltm->tm_sec << std::endl << std::endl;
    
        std::cout << "Number of requested cities: " << ncities << std::endl << std::endl;
        
        //store all data in one huge vector
        std::vector< std::vector<double> > dataSet;
        bool isValidDataSet = true;
        
        //go through the city list and call the data from server
        for (int i = 0; i < ncities; i++) {
            std::string cityID = interface.getVectorEntry("CITYIDS", i);
    
            // webAddress and appID
            std::string webAddress = "http://api.openweathermap.org/data/2.5/weather";
            std::string appID = "4f981d5e51ed9a7f23de89ef80564254";
        
            // build web API string 
            std::string apiAddress = webAddress.append("?id=");
            apiAddress = apiAddress.append(cityID);
            apiAddress = apiAddress.append("&appid="); // &units=metric
            apiAddress = apiAddress.append(appID);
            //std::cout << apiAddress << std::endl;
    
            CURLplusplus client;
            std::string data = client.get(apiAddress);
            //std::cout << data << std::endl;
            JData j(data);

            std::vector<double> jdata = j.getData();
            
            if(jdata.size() < NOBS) {
                isValidDataSet = false;
            }
            dataSet.push_back(jdata);
        }
        
        //write cached data to file
        if(isValidDataSet) {
            for (int i = 0; i < ncities; i++) {
                if(dataSet.size() >= ncities) {
                    std::vector<double> jdata = dataSet[i];
                    if(jdata.size() >= NOBS) {
                        for (int j = 0; j < NOBS; j++) {
                            datafile << jdata[j] << " ";
                        }
                        //screen output
                        std::string cityID = interface.getVectorEntry("CITYIDS", i);
                        std::cout << "Weather in " << cityID /*<< ", " << j.getCityName()*/ << ":" << std::endl;
                        std::cout << "Temperature: " << jdata[0] << " K" << std::endl;
                        std::cout << "Pressure: " << jdata[1] << " hPa" << std::endl;
                        std::cout << "Humidity: " << jdata[2] << " \%" << std::endl;
                        std::cout << "Wind speed: " << jdata[3] << " m/s" << std::endl;
                        std::cout << "Wind direction: " << jdata[4] << " deg" << std::endl;
                        std::cout << "Cloudiness: " << jdata[5] << " \%" << std::endl;
                        std::cout << std::endl;
                    }
                }
            }
            
            datafile << std::endl;
        
            int wait = (int)60*60*24/CALLSPERDAY;
            sleep(wait); //wait
            
        } else {
            // if no valid data set (f.e. not all values could be read), try again in 10 seconds
            std::cout << "Warning: no valid data set, will try again in 10s." << std::endl;
            sleep(10); //wait
        }
    }
    
    datafile.close();
    return 0;
}