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

int main(int argc, const char * argv[]) {
    std::cout << "~~ Weather forecasting ANN ~~" << std::endl;
    
    // load config file
    Interface interface("weather.config");
    std::cout << "Interface: weather.config" << std::endl;
    
    // configuration
    int ncities = interface.getBlockSize("CITYIDS");
    std::cout << "ncities = " << ncities << std::endl;
    int callsPerDay = strToInt(interface.getScalarEntry("CALLSPERDAY"));
    std::cout << "callsPerDay = " << callsPerDay << std::endl;
    int nObs = strToInt(interface.getScalarEntry("NOBSERVABLES"));
    std::cout << "nObs = " << nObs << std::endl;
    int hoursForecast = strToInt(interface.getScalarEntry("FORECAST"));
    std::cout << "hoursForecast = " << hoursForecast << std::endl;
    int nNeurons = strToInt(interface.getScalarEntry("NNEURONS"));
    std::cout << "nNeurons = " << nNeurons << std::endl;
    int eps = strToInt(interface.getScalarEntry("EPS"));
    std::cout << "eps = " << eps << std::endl;
    
    // load output file
    std::fstream datafile, trainfile;
    std::string filename = std::string("weather_");
    filename.append(intToStr(callsPerDay));
    filename.append(".dat");
    std::cout << "Opening " << filename << std::endl;
    datafile.open(filename, std::fstream::in);

    //open additional file with training data
    trainfile.open("train.dat", std::fstream::in | std::fstream::out | std::fstream::trunc);
    
    //read line which was written 24h before and append to learning data 
    // -> this is the output we want to train the ANN for
    std::string line;
    std::vector<std::string> linev;
    datafile.seekg(0, std::fstream::beg);
    while(!datafile.eof()) {
        std::getline(datafile, line);
        std::string firstchr = line.substr(0,1);
        if(firstchr.compare("#") != 0) {
            std::cout << line << std::endl;
            linev.push_back(line);
        }
    }
    // reposition the cursor to eof
    datafile.seekg(std::fstream::end);
    
    //new data string, line by line
    std::vector<std::string> newdatav;
    
    //write training data if enough data is available
    if(linev.size() >= (int)(2*hoursForecast*callsPerDay/24)) {
        std::cout << "-> build training data." << std::endl;
        //cycle line by line
        for(int j = linev.size() - 1; j >= (int)(hoursForecast*callsPerDay/24); j--) {
            //TODO: if '\n' was read, continue with the line before (could be the eof)
            //TODO: check bounds
            std::vector<std::string> wordv = strToVec(linev[j]);
            double ctime = strToDouble(wordv[0]);
        
            std::string buildstr = linev[j];
            //std::cout << buildstr << std::endl;
        
            //go back (hoursForecast*callsPerDay/24) lines and read the line
            //TODO: check bounds
            wordv = strToVec(linev[j - (int)(hoursForecast*callsPerDay/24)]);
            //check for data consistency
            if(strToDouble(wordv[0]) >= (ctime - eps) && 
                strToDouble(wordv[0]) <= (ctime + eps)) {
                //the observables start at position 1 and end at nObs + 1
                for (int k = 1; k < nObs + 1; k++) {
                    buildstr.append(wordv[k]);
                    buildstr.append(" ");
                }
            } else {
                std::cout << "error in datafile. stop." << std::endl;
                std::cout << wordv[0] << " not in [" << ctime - eps << ", " << ctime + eps << std::endl;
                return 1;
                //TODO: instead of an error, check the callsPerDay/2 lines before and after the current line and pick the one which fits best
                //for(int i = j - (int)(hoursForecast*callsPerDay/24) + (int)callsPerDay/2; i >= 0; i--) {
            }
            //std::cout << buildstr << std::endl;
            newdatav.push_back(buildstr);
        }
    
        //normalize data and write out
        //std::vector<double> norm = {323.0, 1100.0, 100.0, 20.0, 360.0, 100.0}; // normalize to [0,1]
        std::vector<double> norm = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // don't normalize data
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
                        //normeddata = (double)strToDouble(wordv[j])/86400.0;
                        normeddata = (double)strToDouble(wordv[j]);
                    } else {
                        normeddata = strToDouble(wordv[j])/norm[(j-1)%nObs];
                    }
                    newdatav[i].append(doubleToStr(normeddata));
                }
                newdatav[i].append(" ");
            }
            trainfile << newdatav[i] << std::endl;
        }
    
        trainfile.close();
        datafile.close();
        
        /*
        // initialize ANN
        // Data set
        DataSet data_set;
        data_set.set_data_file_name("train.dat");
        data_set.set_separator("Space");
        data_set.load_data();
        Variables* variables_pointer = data_set.get_variables_pointer();
        variables_pointer->set(ncities*nObs + 1, nObs);
    
        std::vector<std::string> ids = {"TEM","PRE","HUM","SPE","DIR","CLO"};
        variables_pointer->set_name(0, "TIME");
        for (int i = 0; i < ncities; i++) {
            for (int j = 0; j < nObs; j++) {
                std::cout << i*nObs+j+1 << ", ids = " << ids[j] + intToStr(i) << std::endl;
                variables_pointer->set_name(i*nObs+j+1, ids[j] + intToStr(i));
            }
        }
        for (int i = 0; i < nObs; i++) {
            std::cout << ncities*nObs+i+1 << ", ids = " << ids[i]+ "OUT" << std::endl;
            variables_pointer->set_name(ncities*nObs+i+1, ids[i] + "OUT");
        }
    
        const Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
        const Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();
    
        // Neural network
        NeuralNetwork neural_network(ncities*nObs + 1, nNeurons, nObs);
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
        Vector<double> inputs(ncities*nObs + 1, 0.0);
        Vector<double> outputs(nObs, 0.0);
        */
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
    } else {
        std::cout << "data not enough, collect more!" << std::endl;
        std::cout << "number of lines = " << linev.size() << std::endl;
        std::cout << "required number = " << 2*hoursForecast*callsPerDay/24 << std::endl;
    }
    return 0;
}