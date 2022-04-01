#include "Scanner.hpp"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

Lexema::Lexema(LexemaType t, string str = "")
{
    type = t;
    lex = str;
};

Scanner::Scanner(string input_str)
{
    st = H;
    str.clear();
    input = input_str;
    next_char();
}

void Scanner::next_char()
{
    cur_char = input[0];
    input.erase(0,1);
}

Lexema Scanner::get_lex()
{
    st = H;
    do {
        switch (st) {
            case H:
                if(isspace(cur_char)) 
                    next_char();
                else if (isalpha(cur_char) || cur_char == '_') {
                    str.clear();
                    str += cur_char;
                    st = ID;
                    next_char();
                } else if (isdigit(cur_char)) {
                    str.clear();
                    str += cur_char;
                    next_char();
                    st = NUM;
                } else if (cur_char == '\'') {
                    str.clear();
                    st = STR;
                    next_char();
                } else if (cur_char == '*') {
                    next_char();
                    return Lexema(LEX_MUL, "*");
                } else if (cur_char == ',') {
                    next_char();
                    return Lexema(LEX_COMMA, ",");
                } else if (cur_char == '+') {
                    next_char();
                    return Lexema(LEX_ADD, "+");
                } else if (cur_char == '-') {
                    next_char();
                    return Lexema(LEX_DEC, "-");
                } else if (cur_char == '/') {
                    next_char();
                    return Lexema(LEX_DIV, "/");
                } else if (cur_char == '%') {
                    next_char();
                    return Lexema(LEX_PERCENT, "%");
                } else if (cur_char == '(') {
                    next_char();
                    return Lexema(LEX_OPBR, "(");
                } else if (cur_char == ')') {
                    next_char();
                    return Lexema(LEX_CLBR, ")");
                } else if (cur_char == '>') {
                    next_char();
                    if (cur_char == '=') {
                        next_char();
                        return Lexema(LEX_BQ, ">=");
                    }
                    else
                        return Lexema(LEX_BIGGER, ">");
                } else if (cur_char == '<') {
                    next_char();
                    if (cur_char == '=') {
                        next_char();
                        return Lexema(LEX_LQ, "<=");
                    }
                    else
                        return Lexema(LEX_LESS, "<");
                } else if (cur_char == '!') {
                    next_char();
                    if (cur_char == '=') {
                        next_char();
                        return Lexema(LEX_NQ, "!=");
                    }
                    else
                        throw cur_char;
                } else if (cur_char == '=') {
                    next_char();
                    return Lexema(LEX_EQ, "=");
                } else if (cur_char == EOF || cur_char == ';' || cur_char == '\0')
                    return Lexema(LEX_END);
                else
                    throw cur_char;
                break;
            case ID:
                if(isalpha(cur_char) || isdigit(cur_char) || cur_char == '_'){
                    str += cur_char;
                    next_char();
                }
                else {
                    st = H;
                    if (str == "SELECT")
                        return Lexema(LEX_SELECT, "SELECT");
                    if (str == "INSERT")
                        return Lexema(LEX_INSERT, "INSERT");
                    if (str == "UPDATE")
                        return Lexema(LEX_UPDATE, "UPDATE");
                    if (str == "DELETE")
                        return Lexema(LEX_DELETE, "DELETE");
                    if (str == "DROP")
                        return Lexema(LEX_DROP, "DROP");
                    if (str == "CREATE")
                        return Lexema(LEX_CREATE, "CREATE");
                    if (str == "WHERE")
                        return Lexema(LEX_WHERE, "WHERE");
                    if (str == "SET")
                        return Lexema(LEX_SET, "SET");
                    if (str == "LIKE")
                        return Lexema(LEX_LIKE, "LIKE");
                    if (str == "IN")
                        return Lexema(LEX_IN, "IN");
                    if (str == "INTO")
                        return Lexema(LEX_INTO, "INTO");
                    if (str == "FROM")
                        return Lexema(LEX_FROM, "FROM");
                    if (str == "NOT")
                        return Lexema(LEX_NOT, "NOT");
                    if (str == "ALL")
                        return Lexema(LEX_ALL, "ALL");
                    if (str == "TABLE")
                        return Lexema(LEX_TABLE, "TABLE");
                    if (str == "TEXT")
                        return Lexema(LEX_TEXT, "TEXT");
                    if (str == "LONG")
                        return Lexema(LEX_LONG, "LONG");
                    if (str == "AND")
                        return Lexema(LEX_AND, "AND");
                    if (str == "OR")
                        return Lexema(LEX_OR, "OR");
                    return Lexema(LEX_ID, str);
                }
                break;
            case NUM: 
                if (isdigit(cur_char)) {
                    str += cur_char;
                    next_char();
                } else {
                    st = H;
                    return Lexema(LEX_NUM, str);
                }
                break;
            case STR:
                str += cur_char;
                next_char();
                if (cur_char == '\'') {
                    st = H;
                    next_char();
                    return Lexema(LEX_STR, str);
                }
            default:
                break;
        }
    } while( true );
}
