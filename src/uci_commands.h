#ifndef UCI_COMMANDS_H
#define UCI_COMMANDS_H

#define AUTHOR "Vincent Yu"
#define ENGINE_NAME "Mr Bob"
#define ENGINE_VERSION "v1.2.0"

#include "bitboard.h"
#include "search.h"
#include "uci.h"

class CommandInterface {
public:
    virtual void execute() = 0;
    virtual ~CommandInterface() = default;
protected:
};



class StartCommand : public CommandInterface {
public:
    void execute() override;
    ~StartCommand() {};
};



class UCICommand : public CommandInterface {
public:
    UCICommand(UCIOptions* options) : options(options) {}
    void execute() override;
    ~UCICommand() {};
private:
    UCIOptions* options = nullptr;
};



class ReadyCommand : public CommandInterface {
public:
    void execute() override;
    ~ReadyCommand() {};
};



class NewGameCommand : public CommandInterface {
public:
    NewGameCommand(Search& s) : s(s) {}
    void execute() override;
    ~NewGameCommand() {};
private:
    Search& s;
};



class GoCommand : public CommandInterface {
public:
    GoCommand(UCIParameters* params, std::string command, Search& s, Bitboard& b) : params(params), command(command), s(s), b(b) {}
    void execute() override;
    ~GoCommand() {};
private:
    UCIParameters* params = nullptr;
    std::string command;
    Search& s;
    Bitboard &b;
};



class PositionCommand : public CommandInterface {
public:
    PositionCommand(std::string command, Bitboard& b) : command(command), b(b) {}
    void execute() override;
    ~PositionCommand() {};
private:
    std::string command;
    Bitboard& b;
};



class UCICommandInvoker {
public:
    void cleanCommand() {
        if (command != nullptr) {
            delete command;
            command = nullptr;
        }
    }

    void setCommand(CommandInterface* command) {
        this->command = command;
    };

    void executeUCI() {
        if (command) {
            command->execute();
        }
    }

private:
    CommandInterface* command = nullptr;
};



#endif