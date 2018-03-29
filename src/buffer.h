#pragma once
#include "common.h"
#include <cstdlib>
#include "line.h"


/** buffer.h
 * A buffer is a collection of lines
 */


class Buffer
{
public:
    enum class Type {
        Default,     // Empty buffer
        Document,    // Saved on disk
        Scratch,     // Temporary buffer
        Test,
    };


    // Members
private:
    /*unique identifier of each buffer for debugging*/
    unsigned int id;
    enum Type type;
    const char* name;
    Line* lines;
    unsigned int linecount;

    // Member functions
public:
    Buffer(enum Type type);
    Buffer(enum Type type, const char* filename);

    void destroy();

    Line& line(unsigned int num);
    const Line& line(unsigned int num) const;

    unsigned int lineCount() const;
    bool saveToDisk();
    bool saveToDiskAs(const char* path);

    void pprint() const;
    void pprintLines() const;

    inline const char* Name() const { return name; }
    inline unsigned int Id() const { return id; }

private:
};
