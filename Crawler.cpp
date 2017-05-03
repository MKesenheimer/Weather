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

int main(int argc, const char * argv[]) {

    // load config file
    Interface interface("weather.config");
    
    // configuration
    int ncities = interface.getBlockSize("CITYIDS");
    int callsPerDay = strToInt(interface.getScalarEntry("CALLSPERDAY"));
    int nObs = interface.getBlockSize("OBSERVABLES");
    
    // load output file
    std::fstream datafile;
    std::string filename = std::string("weather_");
    filename.append(intToStr(callsPerDay));
    filename.append(".dat");
    std::cout << "Opening " << filename << std::endl;
    datafile.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    
    // webAddress and appID
    std::string webAddress = "http://api.openweathermap.org/data/2.5/weather";
    std::string appID = "4f981d5e51ed9a7f23de89ef80564254";
    
    // get one data point and store the names
    std::vector<std::string> cityNames(ncities);
    datafile << "# City IDs and Names: " << std::endl;
    for (int i = 0; i < ncities; i++) {
        std::string cityID = interface.getVectorEntry("CITYIDS", i);
        
        // build web API string
        std::string apiAddress = webAddress;
        apiAddress = apiAddress.append("?id=");
        apiAddress = apiAddress.append(cityID);
        apiAddress = apiAddress.append("&appid="); // &units=metric
        apiAddress = apiAddress.append(appID);
    
        CURLplusplus client;
        std::string data = client.get(apiAddress);
        //std::cout << data << std::endl;
        JData j(data);
        cityNames[i] = j.getCityName();
        
        //write out
        datafile << "# (" << i + 1 << ") " << cityID << ", " << cityNames[i] << std::endl;
    }
    
    std::vector<std::string> ids;
    for (int i = 0; i < nObs; i++) {
      ids.push_back(interface.getVectorEntry("OBSERVABLES", i));
    }
    datafile << "# Observables: Time, ";
    for (int i = 0; i < ncities; i++) {
        for (int j = 0; j < nObs; j++) {
            datafile << ids[j] << "(" << i + 1 << ")";
            if (i * nObs + j < ncities * nObs - 1) datafile << ", ";
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
        
            // build web API string 
            // build web API string
            std::string apiAddress = webAddress;
            apiAddress = apiAddress.append("?id=");
            apiAddress = apiAddress.append(cityID);
            apiAddress = apiAddress.append("&appid="); // &units=metric
            apiAddress = apiAddress.append(appID);
    
            CURLplusplus client;
            std::string data = client.get(apiAddress);
            //std::cout << data << std::endl;
            JData j(data);

            std::vector<double> jdata = j.getData();
            cityNames[i] = j.getCityName();
            
            if(jdata.size() < nObs) {
                isValidDataSet = false;
            } else {
                //reorder vector jdata to the user requested format (config OBSERVABLES Block)
                std::vector<double> jdata_bak = jdata;
                for (int i = 0; i < nObs; i++) {
                    if (ids[i].compare("Tem") == 0) {
                        jdata[i] = jdata_bak[0];
                    } else if (ids[i].compare("Pre") == 0) {
                        jdata[i] = jdata_bak[1];
                    } else if (ids[i].compare("Hum") == 0) {
                        jdata[i] = jdata_bak[2];
                    } else if (ids[i].compare("Spe") == 0) {
                        jdata[i] = jdata_bak[3];
                    } else if (ids[i].compare("Dir") == 0) {
                        jdata[i] = jdata_bak[4];
                    } else if (ids[i].compare("Clo") == 0) {
                        jdata[i] = jdata_bak[5];
                    } else {
                        std::cout << "Observable identifier " << ids[i] << " unknown. Stop." << std::endl;
                        exit(-1);
                    }
                }
                dataSet.push_back(jdata);
            }
        }
        
        //write cached data to file
        if(isValidDataSet) {
            datafile << dstamp << " ";
            for (int i = 0; i < ncities; i++) {
                if(dataSet.size() >= ncities) {
                    std::vector<double> jdata = dataSet[i];
                    if(jdata.size() >= nObs) {
                        for (int j = 0; j < nObs; j++) {
                            datafile << jdata[j] << " ";
                        }
                        //screen output
                        std::string cityID = interface.getVectorEntry("CITYIDS", i);
                        std::cout << "Weather in " << cityID << ", " << cityNames[i] << ":" << std::endl;
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
        
            int wait = (int)60*60*24/callsPerDay;
            if (wait/60 == 1) {
              std::cout << "Waiting " << wait/60 << " minute." << std::endl;
            } else {
              std::cout << "Waiting " << wait/60 << " minute." << std::endl;
            }
            sleep(wait); //wait
            
        } else {
            // if no valid data set (f.e. not all values could be read), try again in 10 seconds
            std::cout << "Warning: no valid data set, will try again in 60s." << std::endl;
            std::cout << std::endl;
            sleep(60); //wait
        }
        
        dataSet.clear();
    }
    
    datafile.close();
    return 0;
}