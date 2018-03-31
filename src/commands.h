#pragma once

struct Command {
    /*Each command has a unique id which corresponds to cmd_str[] */
    enum class Type {
        Bufsave = 0,
        Bufopen,
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
