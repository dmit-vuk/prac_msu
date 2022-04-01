#ifndef SCAN
#define SCAN

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

enum LexemaType {
        LEX_SELECT, //0
        LEX_INSERT, //1
        LEX_UPDATE, //2
        LEX_DELETE, //3
        LEX_CREATE, //4
        LEX_DROP, //5 
        LEX_WHERE,//6
        LEX_SET, //7
        LEX_LIKE, //8
        LEX_IN, //9
        LEX_INTO, //10
        LEX_FROM,//11
        LEX_NOT, //12
        LEX_AND, //13
        LEX_OR, //14
        LEX_ALL, //15
        LEX_TABLE, //16
        LEX_TEXT, //17
        LEX_LONG, //18
        LEX_MUL, //19  *
        LEX_DEC, //20  -
        LEX_ADD, //21  +
        LEX_DIV, // 22  /
        LEX_PERCENT, // 23  %
        LEX_BIGGER, // 24  >
        LEX_LESS, //25   <
        LEX_EQ, //26   =
        LEX_NQ, // 27  !=
        LEX_BQ, // 28  >=
        LEX_LQ, // 29  <=
        LEX_OPBR, // 30 (
        LEX_CLBR, // 31 )
        LEX_COMMA, //32  ,
        LEX_STR,//33
        LEX_ID, //34
        LEX_NUM, //35
        BOOL,//36
        LEX_END //37
};

struct Lexema {
    LexemaType type;
    string lex;
    
    Lexema(LexemaType, string);
    Lexema() = default;
};

class Scanner {
    enum State {H, ID, NUM, STR};
    State st;
    int cur_char;
    string str;
    string input;
    
    void next_char();
public:
    Scanner(string);
    Scanner() = default;
    void operator= (Scanner s) {input = s.input; cur_char = s.cur_char;}
    
    Lexema get_lex();
};

#endif
