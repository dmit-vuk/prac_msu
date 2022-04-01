#ifndef TABLE
#define TABLE

extern "C" {
    #include "tables.h"
}
#include <string>

using std::string;


class Table {
    string name;
    THandle table;
public:
    Table(string);
    Table(string, FieldDef *, size_t);
    
    
    string get_name();
    
    void delete_table();
    void open();
    void close();
    void move_first();
    void move_last();
    void move_next();
    void move_previos();    
    bool before_first();
    bool after_last();
    string get_text(const string);
    long get_long(const string);
    void put_text(const string, const string);
    void put_long(const string, long);
    void create_new();
    void put_text_new(const string, const string);
    void put_long_new(const string, long);
    void insert_new();
    void inserta_new();
    void insertz_new();
    void delete_rec();
    unsigned get_field_len(const string);
    FieldType get_field_type(const string);
    unsigned get_fields_num();
    string get_field_name(unsigned);
    
};

#endif
