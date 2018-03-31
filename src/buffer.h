#pragma once
#include "common.h"
#include "line.h"
#include <cstdlib>


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
    //Buffer(enum Type type, const char* filename);
    Buffer(enum Type type, const char* name, Line* lines,
           unsigned int linecount);

    void destroy();

    Line& line(unsigned int num);
    const Line& line(unsigned int num) const;

    unsigned int lineCount() const;

    void pprint() const;
    void pprintLines() const;

    inline const char* getName() const { return name; }
    inline unsigned int Id() const { return id; }

private:
};
