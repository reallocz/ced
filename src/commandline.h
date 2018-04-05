#pragma once

#define CMDBUFCAP 128


struct Command {
    /*Each command has a unique id which corresponds to cmd_str[] */
    enum class Type {
        Bufsave = 0,
        Bufopen,
        Quit,
        Unknown,
        _count,    // Number of commands (last element)
    };

    // Members
    bool valid;        // 0 if invalid
    enum Type type;    // Use only if valid
    char cmd[16]{};
    char args[128]{};

    Command();
    Command(const char* cmdstr);

    void pprint() const;

private:
    // Seperate string to cmd and args
    void parseArgs(const char* cmdstr);
    // Identify the type of command
    void idType();
};


class CommandLine
{
    char buffer[CMDBUFCAP]{};
    unsigned int cur;

public:
    CommandLine();
    void addCh(char ch);
    void delCh();
    void clear();
    Command parse();
    const char* data() const { return buffer; }
    unsigned int getCursor() const { return cur; }
};
