#ifndef PARSER
#define PARSER

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "Scanner.hpp"
#include "Table.hpp"
#include "Poliz.hpp"

using std::stack;
using std::string;
using std::vector;

class Parser {
    string input;
    Lexema curr_lex;
    LexemaType curr_type;
    Scanner scan;
    stack<LexemaType> types;
    
    void start(Poliz &);
    void select(Poliz &);
    void insert(Poliz &);
    void update(Poliz &);
    void delete_tab(Poliz &);
    void create(Poliz &);
    void drop(Poliz &); 
    void where(Poliz &);
    
    void list_fields(Poliz &);
    void name_table(Poliz &);
    void field_value(Poliz &);
    void field_name(Poliz &);
    void expression(Poliz &, bool);
    void list_descriptions(Poliz &);
    void field_type(Poliz &);
    void conditions(Poliz &);
    void string_sample(Poliz &);
    void list_const(Poliz &);
    void term(Poliz &, bool);
    void factor(Poliz &, bool);
    void attitude(Poliz &, bool);
    void value(Poliz &, bool);
    
    void get_lex ();
public:
    bool analyze(Poliz &);
    Parser(string);
};


#endif

