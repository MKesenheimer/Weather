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
#include "opennn.h"

using namespace OpenNN;

//TODO move to weather.config
//get weather data x times per day
#define CALLSPERDAY 24
//forecast weather in x hours
#define FORECAST 24
//the data is checked for consistency, if the recalled data is in an 
//intervall [currentTime - FORECAST - EPS, currenTime - FORECAST + EPS] all is fine.
//EPS in seconds
#define EPS 60*60
//extend data file with current data
#define EXT true
//number of observables
#define NOBS 3

#define NNEURONS 100

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
    datafile << std::setprecision(1) << std::fixed; //<< std::setw(5); //<< std::setprecision(3);

    while (1) {

        //open additional file with training data
        trainfile.open("train.dat", std::fstream::in | std::fstream::out | std::fstream::trunc);
        if(EXT) datafile << std::endl; //prepare new line
        
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
            
            std::cout << "Weather in " << cityID << ", " << j.getCityName() << ":" << std::endl;
            std::cout << "Temperature: " << jdata[0] << " K" << std::endl;
            std::cout << "Pressure: " << jdata[1] << " hPa" << std::endl;
            std::cout << "Humidity: " << jdata[2] << " \%" << std::endl;
            std::cout << "Wind speed: " << jdata[3] << " m/s" << std::endl;
            std::cout << "Wind direction: " << jdata[4] << " deg" << std::endl;
            std::cout << "Cloudiness: " << jdata[5] << " \%" << std::endl;
            std::cout << std::endl;
            
            //write cached data to file        
            if(EXT) {
                for (int j = 0; j < NOBS; j++) {
                    datafile << jdata[j] << " ";
                }
            }
        }
    
        sleep(5*60); //wait 5 minutes
    }
    
    //read line which was written 24h before and append to learning data 
    // -> this is the output we want to train the ANN for
    std::string line;
    std::vector<std::string> linev;
    datafile.seekg(0, std::fstream::beg);
    while(!datafile.eof()) {
        std::getline(datafile, line);
        //std::cout << line << std::endl;
        linev.push_back(line);
    }
    // reposition the cursor to eof
    datafile.seekg(std::fstream::end);
    
    //new data string, line by line
    std::vector<std::string> newdatav;
    
    //write training data if enough data is available
    if(linev.size() >= (int)(2*FORECAST*CALLSPERDAY/24)) {
        //cycle line by line
        for(int j = linev.size() - 1; j >= (int)(FORECAST*CALLSPERDAY/24); j--) {
            //TODO: if '\n' was read, continue with the line before (could be the eof)
            //TODO: check bounds
            std::vector<std::string> wordv = strToVec(linev[j]);
            double ctime = strToDouble(wordv[0]);
        
            std::string buildstr = linev[j];
            //std::cout << buildstr << std::endl;
        
            //go back (FORECAST*CALLSPERDAY/24) lines and read the line
            //TODO: check bounds
            wordv = strToVec(linev[j - (int)(FORECAST*CALLSPERDAY/24)]);
            //check for data consistency
            if(strToDouble(wordv[0]) >= (ctime - EPS) && 
                strToDouble(wordv[0]) <= (ctime + EPS)) {
                //the observables start at position 1 and end at NOBS + 1
                for (int k = 1; k < NOBS + 1; k++) {
                    buildstr.append(wordv[k]);
                    buildstr.append(" ");
                }
            } else {
                std::cout << "error in datafile. stop." << std::endl;
                std::cout << wordv[0] << " not in [" << ctime - EPS << ", " << ctime + EPS << std::endl;
                return 1;
                //TODO: instead of an error, check the CALLSPERDAY/2 lines before and after the current line and pick the one which fits best
                //for(int i = j - (int)(FORECAST*CALLSPERDAY/24) + (int)CALLSPERDAY/2; i >= 0; i--) {
            }
            //std::cout << buildstr << std::endl;
            newdatav.push_back(buildstr);
        }
    
        //normalize data and write out
        std::vector<double> norm = {323.0, 1100.0, 100.0, 20.0, 360.0, 100.0};
        for (int i = 0; i < newdatav.size(); i++) {
            //remove NaNs
            std::vector<std::string> wordv = strToVec(newdatav[i]);
            newdatav[i] = "";
            for(int j = 0; j < wordv.size(); j++) {
                if(wordv[j].compare("nan") == 0) {
                    wordv[j] = "-1.0";
                    newdatav[i].append(wordv[j]);
                } else {
                    double normeddata;
                    if (j == 0) {
                        normeddata = (double)strToDouble(wordv[j])/86400.0;
                    } else {
                        normeddata = strToDouble(wordv[j])/norm[(j-1)%NOBS];
                    }
                    newdatav[i].append(doubleToStr(normeddata));
                }
                newdatav[i].append(" ");
            }
            trainfile << newdatav[i] << std::endl;
        }
    
        trainfile.close();
        datafile.close();
        
        // initialize ANN
        // Data set
        DataSet data_set;
        data_set.set_data_file_name("train.dat");
        data_set.load_data();
        Variables* variables_pointer = data_set.get_variables_pointer();
        variables_pointer->set(ncities*NOBS + 1, NOBS);
    
        std::vector<std::string> ids = {"TEM","PRE","HUM","SPE","DIR","CLO"};
        variables_pointer->set_name(0, "TIME");
        for (int i = 0; i < ncities; i++) {
            for (int j = 0; j < NOBS; j++) {
                std::cout << i*NOBS+j+1 <<", ids = "<<ids[j] + intToStr(i)<<std::endl;
                variables_pointer->set_name(i*NOBS+j+1, ids[j] + intToStr(i));
            }
        }
        for (int i = 0; i < NOBS; i++) {
            std::cout << ncities*NOBS+i+1<<", ids ="<<ids[i]+ "OUT"<<std::endl;
            variables_pointer->set_name(ncities*NOBS+i+1, ids[i] + "OUT");
        }
    
        const Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
        const Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();
    
        // Neural network
        NeuralNetwork neural_network(ncities*NOBS + 1, NNEURONS, NOBS);
        Inputs* inputs_pointer = neural_network.get_inputs_pointer();
        inputs_pointer->set_information(inputs_information);
        Outputs* outputs_pointer = neural_network.get_outputs_pointer();
        outputs_pointer->set_information(targets_information);
    
        // Performance functional
        PerformanceFunctional performance_functional(&neural_network, &data_set);
    
        // Training strategy
        TrainingStrategy training_strategy(&performance_functional);
        training_strategy.perform_training();
    
        // Save results
        data_set.save("data_set.xml");
        neural_network.save("neural_network.xml");
        performance_functional.save("performance_functional.xml");
        training_strategy.save("training_strategy.xml");
    
        // Print results to screen
        Vector<double> inputs(ncities*NOBS + 1, 0.0);
        Vector<double> outputs(NOBS, 0.0);
    
        /*
        std::cout << "X Y AND" << std::endl;
    
        inputs[0] = 0.9;
        inputs[1] = 0.9;
        outputs = neural_network.calculate_outputs(inputs);
        std::cout << inputs << " " << outputs << std::endl;
    
        inputs[0] = 0.9;
        inputs[1] = 0.2;
        outputs = neural_network.calculate_outputs(inputs);
        std::cout << inputs << " " << outputs << std::endl;
    
        inputs[0] = 0.1;
        inputs[1] = 0.9;
        outputs = neural_network.calculate_outputs(inputs);
        std::cout << inputs << " " << outputs << std::endl;
    
        inputs[0] = 0.2;
        inputs[1] = 0.1;
        outputs = neural_network.calculate_outputs(inputs);
        std::cout << inputs << " " << outputs << std::endl;
        */
    }
    return 0;
}