#include "uci.h"
#include <regex>



/*
-----------------------UCIInterface Class-----------------------
*/



UCIInterface::UCIInterface(std::string name) : name(name) {}



std::string UCIInterface::getName() {
    return name;
}



/*
-----------------------SetOptionInt Class-----------------------
*/



bool OptionInt::setParameter(std::string command) {
    std::string tempName = name;
    std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    std::regex sp ("setoption\\sname\\s" + tempName + "\\svalue\\s(\\d+)");
    std::smatch m;

    if (std::regex_search(command, m, sp)) {
        func(std::stoi(m[1]));
        return true;
    }
    else {
        return false;
    }
    
}



void OptionInt::printOption() {
    std::cout << "option name " << name << " type spin default " << def << " min " << min << " max " << max << std::endl;
}



/*
-----------------------SetOptionString Class-----------------------
*/



bool OptionString::setParameter(std::string command) {
    std::string tempName = name;
    std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    

    std::regex sp ("setoption\\sname\\s" + tempName + "\\svalue\\s(.+)");
    std::smatch m;

    if (std::regex_search(command, m, sp)) {
        func(m[1]);
        return true;
    }
    else {
        return false;
    }
    
}



void OptionString::printOption() {
    std::cout << "option name " << name << " type string default " << def << std::endl;
}



/*
-----------------------ParameterInt Class-----------------------
*/



std::string ParameterInt::getName() {
    return name;
}



int ParameterInt::getParameter(std::string command) {
    std::string tempName = name;
    std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    

    std::regex sp (".*" + tempName + "\\s(\\d+).*");
    std::smatch m;

    if (std::regex_search(command, m, sp)) {
        return std::stoi(m[1]);
    }
    else {
        return -1;
    }
}





/*
-----------------------UCIOptions Class-----------------------
*/



void UCIOptions::setOption(std::string command) {
    for (auto option : options) {
        if (option->setParameter(command)) {
            return;
        }
    }

    throw std::invalid_argument("Unrecognized argument name");
}



void UCIOptions::printAllOptions() {
    for (auto option : options) {
        option->printOption();
    }
}



/*
-----------------------UCIParameters Class-----------------------
*/



std::vector<UCIParameters::params> UCIParameters::getParameters(std::string command) {
    std::vector<UCIParameters::params> ret;
    for (auto param : parameters) {
        int val = param->getParameter(command);
        if (val != -1) {
            UCIParameters::params newParam;
            newParam.name = param->getName();
            newParam.val = val;
            ret.push_back(newParam);
        }
    }

    return ret;
}



void UCIParameters::addParameter(std::string name) {
    parameters.push_back(new ParameterInt(name));
}


