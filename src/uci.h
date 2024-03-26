#ifndef UCI_H
#define UCI_H

#include <iostream>
#include <vector>
#include <functional>



class UCIInterface {

public:
    UCIInterface(std::string name);
    std::string getName();
    virtual bool setParameter(std::string command) = 0;
    virtual void printOption() = 0;
protected:
    std::string name;
};



class OptionInt : public UCIInterface {

public:
    OptionInt(std::function<void(int)> func, std::string name, int def, int min, int max) : 
        UCIInterface(name), func(func), def(def), min(min), max(max) {};
    bool setParameter(std::string command);
    void printOption();
private:
    std::function<void(int)> func;
    int def;
    int min;
    int max;
};


class OptionString : public UCIInterface {

public:
    OptionString(std::function<void(std::string)> func, std::string name, std::string def) : 
        UCIInterface(name), func(func), def(def) {};
    bool setParameter(std::string command);
    void printOption();
private:
    std::function<void(std::string)> func;
    std::string def;
};



class ParameterInt {

public:
    ParameterInt(std::string name) : name(name) {};
    std::string getName();
    int getParameter(std::string command);
private:
    std::string name;
};



class UCIOptions {
public:

    template <typename ClassType>
    void addOption(ClassType func, std::string name, int def = 1, int min = 1, int max = 128) {
        options.push_back(new OptionInt(func, name, def, min, max));
    }

    template <typename ClassType>
    void addOption(ClassType func, std::string name, std::string def = "default") {
        options.push_back(new OptionString(func, name, def));
    }

    void setOption(std::string command);

    void printAllOptions();
private:
    std::vector<UCIInterface*> options;
};



class UCIParameters {
public:
    struct params {
        std::string name;
        int val;
    };
    void addParameter(std::string name);
    std::vector<UCIParameters::params> getParameters(std::string command);
private:
    std::vector<ParameterInt*> parameters;

};



#endif