#pragma once
#include "common.h"

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
    struct Line {
        unsigned int len; /** length of line */
        char* data;       /** line data */
    };
    struct Gap {
        unsigned int line;
        unsigned int col;
        unsigned int size;
    };

    // Members
private:
    /*unique identifier of each buffer for debugging*/
    unsigned int id;
    enum Type type;
    const char* name;
    Gap gap;
    Line* lines;
    unsigned int linecount;


    // Member functions
public:
    Buffer(enum Type type);
    Buffer(enum Type type, const char* filename);

    void destroy();

    void addCh(char ch, const struct cursor& cur);
    void delCh(const struct cursor& cur);

    Line* line(unsigned int num);
    const Line* line(unsigned int num) const;
    unsigned int lineCount() const;
    bool lineHasGap(unsigned int line) const;
    bool inGap(unsigned int i) const;
    bool saveToDisk(const char* path);

    void pprint() const;
    void pprintLine(unsigned int i) const;
    void pprintLines() const;

    inline const char* Name() const { return name; }
    inline Buffer::Gap Gap() const { return gap; }
    inline unsigned int Id() const { return id; }

private:
    bool addGap();
    bool addGapOptional();
    bool moveGap(const struct cursor& cur);
    bool moveGapToLine(unsigned int toline);
};
