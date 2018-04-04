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
    size_t id;
    enum Type type;
    const char* name;
    Line* lines;
    size_t linecount;

    // Member functions
public:
    Buffer(enum Type type);
    // Buffer(enum Type type, const char* filename);
    Buffer(enum Type type, const char* name, Line* lines,
           size_t linecount);

    void destroy();

    inline const char* getName() const { return name; }
    inline size_t Id() const { return id; }
    size_t lineCount() const { return linecount; };

    // Line
    Line& getLine(size_t num);
    const Line& getLine(size_t num) const;
    void addLineAt(size_t idx, Line& ln);

    void pprint() const;
    void pprintLines() const;

private:
};
