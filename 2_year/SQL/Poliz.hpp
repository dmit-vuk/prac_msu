#ifndef POLIZ 
#define POLIZ

#include "Scanner.hpp"
#include "Table.hpp"
#include <iostream>
#include <vector>
#include <string>

using std::string;
using std::vector;

class Poliz {
    vector<Lexema> pol;
    vector<Lexema> where_poliz;
    
    vector<string> select();
    vector<string> insert();
    vector<string> update();
    vector<string> delete_rec();
    vector<string> create();
    vector<string> drop(); 
    
    bool check_num(Lexema, Table &);
    bool check_str(Lexema, Table &);
    bool diff_types(Lexema, Lexema, Table &);
    long get_num(Lexema, Table &);
    string get_str(Lexema, Table &);
    bool get_bool(Lexema, Table &);
    Lexema calc_expr(vector<Lexema> &, Table &, size_t &);
    
    void where(vector<bool> &, Table &);
    void in(FieldType, vector<bool> &, Table &, size_t);
    void logic(Table &, vector<bool> &);
    void like(vector<bool> &, Table &);

public:
    vector<string> calculate();
    
    void set(Lexema);
    void set_wh(Lexema);
    
    size_t size();
};

#endif 

